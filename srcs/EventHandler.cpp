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
int totale = 0;
static void handleReceivedData(Server &server, epoll_event event) {
	char buffer[1024];
	Socket &client = server.sockets[event.data.fd];
	std::string &request = client.request.request;
	int n = recv(event.data.fd, buffer, sizeof(buffer), MSG_NOSIGNAL);
	if (n == -1)
		return Log::Error("recv failed");
	request.reserve(n);
	for (int i = 0; i < n; i++)
		request += buffer[i];
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT;
	server.modifyPoll(event.data.fd, event);
}

static void	sendResponse(Server &server, Socket &client, epoll_event event) {
	Request &request = client.request;
	ServerConfig &config = server.serverConfigs[client.getServerFd()];
	Response response(client, config);

	request.clear();
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
		Socket &client = server.sockets[event.data.fd];
		client.request.parseRequest();
		if (client.request.resCode == 202)
			dprintf(event.data.fd, "HTTP/1.1 202 Accepted\nContent-Type: text/html\nContent-Length: 0\n\n");
		if (client.request.isReqGenerated == true)
			sendResponse(server, client, event);
	}
}
