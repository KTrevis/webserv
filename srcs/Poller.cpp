#include "Poller.hpp"
#include <iostream>
#include <sys/poll.h>
#include <errno.h>
#include <unistd.h>

void	Poller::poll() {
	struct pollfd arr[this->sockets.size()];

	for (size_t i = 0; i < this->sockets.size(); i++)
		arr[i] = this->sockets[i];
	::poll(arr, this->sockets.size(), 0);
}

void	Poller::addSocket(int fd) {
	struct pollfd pollfd;
	pollfd.fd = fd;
	this->sockets.push_back(pollfd);
}
