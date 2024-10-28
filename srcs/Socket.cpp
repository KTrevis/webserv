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
	_cgiFd[0] = -1;
	_cgiFd[1] = -1;
}

Socket::Socket(int fd) {
	_serverFd = fd;
	_fd = fd;
	_cgiFd[0] = -1;
	_cgiFd[1] = -1;
	Log::Trace(StringUtils::itoa(_fd) + " socket created");
}

int	Socket::getFd() const {
	return _fd;
}

int	Socket::getServerFd() const {
	return _serverFd;
}

const int	(&Socket::getCgiFd() const)[2] {
	return _cgiFd;
}

Socket::~Socket() {
	if (_fd < 0) return;
	close(_fd);
	std::string err = StringUtils::itoa(_fd);
	err += _serverFd == _fd ? " server": " client";
	err += " socket closed";
	Log::Trace(err);
}

void	Socket::setup(int fd, int serverFd, ServerConfig &config) {
	request.config = config;
	_fd = fd;
	_serverFd = serverFd;
}

bool	Socket::isServer() {
	return _serverFd == _fd;
}

void	Socket::createPipe() {
	pipe(_cgiFd);
}
