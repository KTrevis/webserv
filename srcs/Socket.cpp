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
	_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	Log::Error(StringUtils::itoa(_fd) + " socket created");
}

Socket::Socket(int fd) {
	_fd = fd;
	Log::Error(StringUtils::itoa(_fd) + " socket created");
}

int	Socket::getFd() const {
	return _fd;
}

Socket::~Socket() {
	if (_fd < 0) return;
	if (close(_fd) < 0) {
		Log::Error(StringUtils::itoa(_fd) + " socket close failed");
		return;
	}
	Log::Debug(StringUtils::itoa(_fd) + " socket closed successfully");
}
