#include "Socket.hpp"
#include <sys/poll.h>
#include <unistd.h>
#include <iostream>

Socket::Socket() {
	_fd = socket(AF_INET, SOCK_STREAM, 0);
}

Socket::Socket(int fd) {
	_fd = fd;
}

int	Socket::getFd() const {
	return _fd;
}

Socket::~Socket() {
	if (close(_fd) < 0)
		std::cerr << "ERROR: " << _fd << " close failed" << std::endl;
	else
		std::cerr << _fd << " socket closed" << std::endl;
}
