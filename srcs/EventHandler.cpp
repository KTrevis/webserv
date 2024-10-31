#include "EventHandler.hpp"
#include "HeaderFields.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "StringUtils.hpp"
#include <cstdio>
#include <ostream>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

static std::string	&getStrToModify(int eventFd, Socket &client, Server &server) {
	std::map<int, Response>::iterator it = server.cgiResponses.find(eventFd);

	if (it == server.cgiResponses.end())
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
	std::map<int, Response>::iterator it = server.cgiResponses.find(client.getFd());

	if (it != server.cgiResponses.end())
		return;
	std::pair<int, Response> pair(client.getFd(), Response(client, config));
	server.cgiResponses.insert(pair);
	it = server.cgiResponses.find(client.getFd());

	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
	Response &response = it->second;
	response.setup();
	event.events += EPOLLOUT;

	request.clear();
	server.modifyPoll(client.getFd(), event);
}

static void handleResponse(Socket &client, Response &response, Server &server, epoll_event &event) {
	if (response.getCGI().getScriptPath() != "") {
		if (!response.getCGI().isReady())
			return;
		response.handleCGI(server);
		return;
	}
	if (!response.fullySent()) {
		response.sendChunk();
		return;
	}
	else {
		event.events -= EPOLLOUT;
		Log::Debug("Chunk fully sent");
		server.cgiResponses.erase(client.getFd());
	}
}

static bool isCGI(int fd, Server &server) {
	return server.cgiResponses.find(fd) != server.cgiResponses.end();
}

void	EventHandler::handleEvent(Server &server, epoll_event &event) {
	Log::Event(event.events);
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
		std::map<int, Response>::iterator it = server.cgiResponses.find(client.getFd());
		Response &response = it->second;

		if (it != server.cgiResponses.end()) {
			handleResponse(client, response, server, event);
			server.modifyPoll(client.getFd(), event);
			return;
		}
		client.request.parseRequest();
		if (client.request.isReqGenerated == true)
			sendResponse(server, client, event);
	}
}
