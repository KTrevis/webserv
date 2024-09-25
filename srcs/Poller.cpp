#include "Poller.hpp"
#include <sys/poll.h>
#include <unistd.h>

Poller::~Poller() {
	
}

void	Poller::poll() {
	struct pollfd arr[this->sockets.size()];

	for (size_t i = 0; i < this->sockets.size(); i++) {
		arr[i].fd = this->sockets[i].getFd();
		arr[i].events = POLLHUP | POLLIN | POLLOUT;
	}
	::poll(arr, this->sockets.size(), 1000);
}

void	Poller::addSocket(int fd) {
	this->sockets.push_back(Socket(fd));
}
