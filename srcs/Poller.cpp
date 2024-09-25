#include "Poller.hpp"
#include <sys/poll.h>
#include <unistd.h>

Poller::~Poller() {
	
}

void	Poller::poll() {
	size_t size = this->sockets.size();
	struct pollfd arr[size];

	for (size_t i = 0; i < size; i++) {
		arr[i].fd = this->sockets[i].getFd();
		arr[i].events = POLLHUP | POLLIN | POLLOUT;
	}
	::poll(arr, size, 1000);
}

void	Poller::addSocket(int fd) {
	this->sockets.push_back(Socket(fd));
}
