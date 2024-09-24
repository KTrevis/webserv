#include "Socket.hpp"
#include <unistd.h>

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
	close(this->fd);
}
