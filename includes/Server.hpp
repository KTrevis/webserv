#pragma once

#include "Socket.hpp"
#include "Epoll.hpp"
#include <map>
#include <vector>

class Server {
	public:
		Server(std::vector<ServerConfig> &arr);
		void	start();
		std::map<int, ServerConfig>	_serverConfigs;
		std::map<int, Socket>		_sockets;
		Server	&operator=(const Server &server);
		bool	parseConfig(ServerConfig &config);
	private:
		Epoll					_epoll;
};
