#include "Socket.hpp"
#include <unistd.h>
#include <iostream>

Socket::Socket() {
	this->fd = socket(AF_INET, SOCK_STREAM, 0);
}

Socket::Socket(int fd) {
	this->fd = fd;
}

int	Socket::getFd() const {
	return this->fd;
}

Socket::~Socket() {
	if (close(this->fd) < 0)
		std::cerr << "ERROR:" << fd << " close failed" << std::endl;
	else
		std::cerr << this->fd << " socket closed" << std::endl;
}
