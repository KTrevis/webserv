#include "EventHandler.hpp"
#include "Epoll.hpp"
#include "Log.hpp"
#include <cstdio>
#include <unistd.h>

static bool handleReceivedData(Epoll &epoll, epoll_event &event) {
	char buffer[1024];
	int n = read(event.data.fd, buffer, 1023);
	if (n == 0 || n == -1) return false;
	buffer[n] = 0;
	Log::Trace(buffer);
	epoll_event modEvent = event;
	modEvent.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT;
	epoll.modifyPoll(event.data.fd, modEvent);
	return true;
}

void	EventHandler::handleEvent(Epoll &epoll, epoll_event &event) {
	Log::Event(event.events);
	if (event.events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		epoll.closeConnection(event);
		return;
	}
	if (event.events & EPOLLIN && epoll.isNewClient(event)) {
		epoll.createNewClient();
		return;
	}
	if (event.events & EPOLLIN) {
		if (!handleReceivedData(epoll, event))
			epoll.closeConnection(event);
	}
	if (event.events & EPOLLOUT) {
			dprintf(event.data.fd, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 66\n\n<html><head><title>Basic Page</title></head><body><h1>Hello, World!</body></html>");
		epoll_event modEvent = event;
		modEvent.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
		epoll.modifyPoll(event.data.fd, modEvent);
	}
}
