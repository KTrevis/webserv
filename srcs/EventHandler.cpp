#include "EventHandler.hpp"
#include "Epoll.hpp"
#include "Log.hpp"
#include <cstdio>
#include <unistd.h>

bool EventHandler::handleReceivedData(epoll_event &event) {
	char str[1024];
	int n = read(event.data.fd, str, 1023);
	if (n == 0 || n == -1) return false;
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
		if (!handleReceivedData(event))
			epoll.closeConnection(event);
	}
	if (event.events & EPOLLOUT)
		dprintf(event.data.fd, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 66\n\n<html><head><title>Basic Page</title></head><body><h1>Hello, World!</body></html>");

}
