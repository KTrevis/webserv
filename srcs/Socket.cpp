#include "StringUtils.hpp"
#include "Socket.hpp"
#include "Log.hpp"
#include <cstdio>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

Socket::Socket() {
	_fd = -1;
}

Socket::Socket(int fd) {
	_fd = fd;
	Log::Trace(StringUtils::itoa(_fd) + " socket created");
}

int	Socket::getFd() const {
	return _fd;
}

Socket::~Socket() {
	if (_fd < 0) return;
	close(_fd);
	std::string err = StringUtils::itoa(_fd);
	err += _isServer ? " server": " client";
	err += " socket closed";
	Log::Trace(err);
}

void	Socket::setup(int fd, bool isServer) {
	_isServer = isServer;
	_fd = fd;
}

bool	Socket::isServer() {
	return _isServer;
}
