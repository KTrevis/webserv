#include "EventHandler.hpp"
#include "HeaderFields.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "StringUtils.hpp"
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
	int n = recv(event.data.fd, buffer, sizeof(buffer), MSG_NOSIGNAL);

	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT;
	server.modifyPoll(event.data.fd, event);
	if (n == -1)
		return Log::Error("recv failed");
	request.reserve(n);
	for (int i = 0; i < n; i++)
		request += buffer[i];
}

static void	sendResponse(Server &server, Socket &client, epoll_event event) {
	Request &request = client.request;
	ServerConfig &config = server.serverConfigs[client.getServerFd()];
	std::map<int, Response>::iterator it = server.responses.find(client.getFd());

	if (it != server.responses.end())
		return;
	try {
		std::pair<int, Response> pair(client.getFd(), Response(client, config));
		server.responses.insert(pair);
		it = server.responses.find(client.getFd());
		it->second.setup();
	} catch(std::exception &e) {
		std::string error = "EventHandler sendResponse: " + std::string(e.what());
		Log::Error(error);
		dprintf(client.getFd(), "%s", StringUtils::createResponse(404).c_str());
	}

	request.clear();
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT;
	server.modifyPoll(client.getFd(), event);
}

static void handleExistingResponse(Socket &client, Response &response, Server &server, epoll_event &event) {
	if (response.getCGI().getScriptPath() != "") {
		if (!response.getCGI().isReady())
			return;
		response.handleCGI(server, event);
		return;
	}
	if (response.fullySent()) {
		event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
		Log::Trace("Chunk fully sent");
		server.responses.erase(client.getFd());
		return;
	}
	response.sendChunk();
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
			handleExistingResponse(client, response, server, event);
			server.modifyPoll(client.getFd(), event);
			return;
		}
		client.request.parseRequest();
		if (client.request.state == SEND_RESPONSE)
			sendResponse(server, client, event);
	}
}
