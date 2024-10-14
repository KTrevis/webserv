#pragma once

#include "Socket.hpp"
#include "Address.hpp"
#include "Epoll.hpp"
#include "ConfigParser.hpp"
#include <map>
#include <vector>

class Server {
	public:
		Server(std::vector<ServerConfig> &arr);
		void	start();
		std::map<int, ServerConfig>	&getServerConfigs();
		std::vector<Socket *>		_serverSockets;
		Server	&operator=(const Server &server);
		bool					parseConfig(ServerConfig &config);
	private:
		std::map<int, ServerConfig>	_serverConfigs;
		std::map<int, Socket>	_clientSockets;
		Epoll					_epoll;
};
