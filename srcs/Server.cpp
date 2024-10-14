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
	Socket *newSocket = new Socket(fd);
	_serverSockets.push_back(newSocket);
	if (NetworkUtils::bind(*newSocket, config.address) == false)
		return false;
	if (listen(fd, 5) == -1)
		return false;
	return true;
}

Server::Server(std::vector<ServerConfig> &arr): _epoll(*this, arr) {}

std::map<int, ServerConfig>	&Server::getServerConfigs() {
	return _serverConfigs;
}
