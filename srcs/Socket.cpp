#include "StringUtils.hpp"
#include "Socket.hpp"
#include "Log.hpp"
#include <cstdio>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket() {
	_fd = -1;
	_serverFd = -1;
	TimedOut = false;
	lastActivity = std::time(NULL);
}

Socket::Socket(int fd) {
	_port = -1;
	_serverFd = fd;
	_fd = fd;
	lastActivity = std::time(NULL);
	TimedOut = false;
	Log::Trace(StringUtils::itoa(_fd) + " socket created");
}

int	Socket::getFd() const {
	return _fd;
}

int	Socket::getServerFd() const {
	return _serverFd;
}

Socket::~Socket() {
	if (_fd < 0) return;
	close(_fd);
	std::string err = StringUtils::itoa(_fd);
	err += _serverFd == _fd ? " server": " client";
	err += " socket closed";
	Log::Trace(err);
}

void	Socket::setup(int fd, int serverFd, int port) {
	_fd = fd;
	_serverFd = serverFd;
	_port = port;
}

bool	Socket::isServer() {
	if (_fd == -1)
		return false;
	return _serverFd == _fd;
}

void	Socket::updateActivity() {
	lastActivity = std::time(NULL);
}

int	Socket::getPort() const {
	return _port;
}
