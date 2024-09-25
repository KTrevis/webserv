#include "Epoll.hpp"
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

Epoll::Epoll(int fdToMonitor) {
	this->epollfd = epoll_create1(0);
	this->events.data.fd = fdToMonitor;
	this->events.events = EPOLLIN | EPOLLOUT;
	epoll_ctl(this->epollfd, EPOLL_CTL_ADD, fdToMonitor, &this->events);
}

void	Epoll::wait() {
	epoll_wait(this->epollfd, &this->events, 100, -1);
	std::cout << "je suis chokbar" << std::endl;
}

Epoll::~Epoll() {
	close(this->epollfd);
}
