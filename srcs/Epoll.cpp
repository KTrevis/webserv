#include "Epoll.hpp"
#include "Server.hpp"
#include <iostream>
#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>

void	Epoll::addFdToPoll(int fd, epoll_event event) {
	epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &event);
}

Epoll::Epoll(Server &server): _server(server) {
	epoll_event event;

	bzero(_events, sizeof(_events));
	_epollfd = epoll_create1(0);
	event.events = EPOLLIN | EPOLLOUT;
	addFdToPoll(server.getFd(), event);
}

bool Epoll::isNewClient(const epoll_event &event) {
	return event.data.fd == _server.getFd();
}

void	Epoll::handleNewConnection() {
	std::cout << "New connexion" << std::endl;
}

void	Epoll::wait() {
	int nbEvents = epoll_wait(_epollfd, _events, MAX_EVENTS, -1);

	for (int i = 0; i < nbEvents; i++) {
		if (isNewClient(_events[i]))
			handleNewConnection();
	}
}

Epoll::~Epoll() {
	close(_epollfd);
}
