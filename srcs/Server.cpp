#include "Server.hpp"
#include "ConfigParser.hpp"
#include "NetworkUtils.hpp"
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

void	Server::start() {
	while (1)
		_epoll.wait();
}

bool Server::parseConfig(ServerConfig &config) {
	int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (fd == -1) return false;
	_serverConfigs[fd] = config;
	_sockets[fd].setup(fd);
	if (NetworkUtils::bind(_sockets[fd], config.address) == false)
		return false;
	if (listen(fd, 5) == -1)
		return false;
	return true;
}

Server::Server(std::vector<ServerConfig> &arr): _epoll(*this, arr) {}

std::map<int, ServerConfig>	&Server::getServerConfigs() {
	return _serverConfigs;
}

const std::map<int, Socket>	&Server::getSockets() {
	return _sockets;
}
