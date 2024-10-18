#include "EventHandler.hpp"
#include "StringUtils.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Request.hpp"
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
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT;
	server.modifyPoll(event.data.fd, event);
	// client.request.parseRequest();
}

static void	sendResponse(Server &server, epoll_event event) {
	const Socket &client = server.sockets[event.data.fd];
	const Request &request = client.request;
	ServerConfig &config = server.serverConfigs[client.getServerFd()];
	const std::string &url = "/";
	std::vector<std::string> path = StringUtils::split(url, "/", true);

	(void)config;
	(void)request;
	if (path.size() == 0)
		path.push_back("/");
	Log::Debug(path[0]);
	Log::Debug(config.locations[path[0]].root);
	dprintf(event.data.fd, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 66\n\n<html><head><title>Basic Page</title></head><body><h1>Hello, World!</body></html>");
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
	server.modifyPoll(event.data.fd, event);
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
		if (server.sockets[event.data.fd].request.request.find("\r\n\r\n"))
			sendResponse(server, event);
	}
}
