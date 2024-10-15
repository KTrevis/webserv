#include "EventHandler.hpp"
#include "Epoll.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include <cstdio>
#include <unistd.h>

Request request;

static void handleReceivedData(Epoll &epoll, epoll_event &event, std::string &req) {
    char buffer[1024];
    std::string request;
	Socket &client = epoll._server._sockets[event.data.fd];
	int n = recv(event.data.fd, buffer, sizeof(buffer), MSG_NOSIGNAL);

	buffer[n] = '\0';
	request.append(buffer, n);
	Log::Trace(req);
	epoll_event modEvent = event;
	modEvent.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT;
	epoll.modifyPoll(event.data.fd, modEvent);
}

void	EventHandler::handleEvent(Epoll &epoll, epoll_event &event) {
	Log::Event(event.events);
	static std::string req;
	if (event.events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		epoll.closeConnection(event);
		return;
	}
	if (event.events & EPOLLIN && epoll.isNewClient(event))
		return;
	if (event.events & EPOLLIN)
		handleReceivedData(epoll, event, req);
	if (event.events & EPOLLOUT) {
		dprintf(event.data.fd, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 66\n\n<html><head><title>Basic Page</title></head><body><h1>Hello, World!</body></html>");
		epoll_event modEvent = event;
		modEvent.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
		epoll.modifyPoll(event.data.fd, modEvent);
	}
}
