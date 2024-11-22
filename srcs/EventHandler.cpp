#include "EventHandler.hpp"
#include "HeaderFields.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "StringUtils.hpp"
#include "Utils.hpp"
#include <cstdio>
#include <exception>
#include <ostream>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

static std::string	&getStrToModify(int eventFd, Socket &client, Server &server) {
	std::map<int, Response>::iterator it = server.responses.find(eventFd);

	if (it == server.responses.end())
		return client.request.request;
	return it->second.getCGI().body;
}

static void handleReceivedData(Server &server, epoll_event event) {
	char buffer[1024];
	Socket &client = server.sockets[event.data.fd];
	std::string &request = getStrToModify(event.data.fd, client, server);
	int n = recv(event.data.fd, buffer, sizeof(buffer), MSG_NOSIGNAL | MSG_DONTWAIT);

	if (n == 0) {
		server.closeConnection(event);
		return;
	}
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR | EPOLLOUT;
	server.modifyPoll(event.data.fd, event);
	if (n == -1)
		return Log::Error("recv failed");
	client.updateActivity();
	request.reserve(n);
	for (int i = 0; i < n; i++)
		request += buffer[i];
	if (client.request.state == IDLE)
		client.request.state = PARSE_METHOD;
}


static void	sendResponse(Server &server, Socket &client, epoll_event event) {
	ServerConfig &config = Utils::getServerConfig(server, client);
	std::map<int, Response>::iterator it = server.responses.find(client.getFd());

	if (it != server.responses.end())
		return;
	std::pair<int, Response> pair(client.getFd(), Response(client, config, server));
	server.responses.insert(pair);
	it = server.responses.find(client.getFd());
	it->second.setup();
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR | EPOLLOUT;
	server.modifyPoll(client.getFd(), event);
}

static void handleExistingResponse(Socket &client, Response &response, Server &server, epoll_event &event) {
	if (client.request.resCode != 0) {
		response.setErrorPage(client.request.resCode);
		client.request.clear();
		client.request.resCode = 0;
	}
	else if (client.request.method != "DELETE" && response.getCGI().getScriptPath() != "") {
		if (!response.getCGI().isReady())
			return;
		response.handleCGI();
		client.request.clear();
	}
	else if (response.fullySent()) {
		event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
		Log::Trace("Chunk fully sent");
		client.request.clear();
		client.request.cgiBody.clear();
		server.responses.erase(client.getFd());
		server.closeConnection(event);
		return;
	}
	response.sendChunk();
	client.updateActivity();
}

static bool isCGI(int fd, Server &server) {
	return server.responses.find(fd) != server.responses.end();
}

void	EventHandler::handleEvent(Server &server, epoll_event &event) {
	if (event.data.fd == server._timerFd) {
		uint64_t expirations;
		read(server._timerFd, &expirations, sizeof(expirations));
		server.checkClientTimeouts();
		return;
	}
	if (event.events & EPOLLIN && !isCGI(event.data.fd, server) && server.isNewClient(event))
		return;
	if (event.events & EPOLLIN)
		handleReceivedData(server, event);
	if (event.events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		server.closeConnection(event);
		return;
	}
	if (event.events & EPOLLOUT) {
		Socket &client = server.sockets[event.data.fd];
		if (client.TimedOut == true)
			server.closeConnection(event);
		std::map<int, Response>::iterator it = server.responses.find(client.getFd());
		Response &response = it->second;

		if (it != server.responses.end()) {
			int fd = client.getFd();
			handleExistingResponse(client, response, server, event);
			server.modifyPoll(fd, event);
			return;
		}
		client.request.parseRequest(server, client);
		if (client.request.state == SEND_RESPONSE)
			sendResponse(server, client, event);
	}
}
