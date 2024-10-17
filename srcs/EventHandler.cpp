#include "EventHandler.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include <cstdio>
#include <unistd.h>

static void handleReceivedData(Server &server, epoll_event event) {
	char buffer[1024];
	Socket &client = server._sockets[event.data.fd];
	std::string &request = client.request;
	int n = recv(event.data.fd, buffer, sizeof(buffer), MSG_NOSIGNAL);

	if (n == -1)
		return Log::Error("recv failed");
	buffer[n] = '\0';
	request += buffer;
	Log::Trace(buffer);
	if (request.find("\r\n"))
		request.clear();
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT;
	server.modifyPoll(event.data.fd, event);
}

void	EventHandler::handleEvent(Server &server, epoll_event &event) {
	Log::Event(event.events);
	if (event.events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		server.closeConnection(event);
		return;
	}
	if (event.events & EPOLLIN && server.isNewClient(event))
		return;
	if (event.events & EPOLLIN)
		handleReceivedData(server, event);
	if (event.events & EPOLLOUT) {
		dprintf(event.data.fd, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 66\n\n<html><head><title>Basic Page</title></head><body><h1>Hello, World!</body></html>");
		epoll_event modEvent = event;
		modEvent.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
		server.modifyPoll(event.data.fd, modEvent);
	}
}
