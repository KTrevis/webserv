#include "Socket.hpp"
#include "Log.hpp"
#include <cstdio>
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
	char log[1024];

	if (close(_fd) < 0) {
		sprintf(log, "%d socket with fd close failed", _fd); 
		Log::Error(log);
		return;
	}
	sprintf(log, "%d socket with fd closed successfully", _fd); 
	Log::Debug(log);
}
