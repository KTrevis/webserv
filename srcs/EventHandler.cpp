#include "EventHandler.hpp"
#include "StringUtils.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <algorithm>
#include <cstdio>
#include <unistd.h>
#include <vector>

static void handleReceivedData(Server &server, epoll_event event) {
	char buffer[1024];
	Socket &client = server.sockets[event.data.fd];
	std::string &request = client.request.request;
	int n = recv(event.data.fd, buffer, sizeof(buffer), MSG_NOSIGNAL);
	if (n == -1)
		return Log::Error("recv failed");
	buffer[n] = '\0';
	request += buffer;
	if (request.find("\r\n\r\n"))
	{
		event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT;
		server.modifyPoll(event.data.fd, event);
	}
}

static void	sendResponse(Server &server, epoll_event event) {
	const Socket &client = server.sockets[event.data.fd];
	const Request &request = client.request;
	ServerConfig &config = server.serverConfigs[client.getServerFd()];
	Response response(client, request, config);

	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
	server.modifyPoll(client.getFd(), event);
}

void	EventHandler::handleEvent(Server &server, epoll_event &event) {
	Log::Event(event.events);
	if (event.events & EPOLLIN && server.isNewClient(event))
		return;
	if (event.events & EPOLLIN)
		handleReceivedData(server, event);
	if (event.events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		server.closeConnection(event);
		return;
	}
	if (event.events & EPOLLOUT) {
		server.sockets[event.data.fd].request.parseRequest();
		// Log::Info(server.sockets[event.data.fd].request.method);
		// Log::Info(server.sockets[event.data.fd].request.path);
		// Log::Info(server.sockets[event.data.fd].request.httpVer);
		// server.sockets[event.data.fd].request.displayArgs();
		if (server.sockets[event.data.fd].request.request.find("\r\n\r\n"))
			sendResponse(server, event);
	}
}
