#pragma once

#include "Socket.hpp"
#include <map>
#include <vector>
#include <sys/epoll.h>
#include <ServerConfig.hpp>

#define MAX_EVENTS 100

class Server {
	public:
		Server(std::vector<ServerConfig> &arr);
		~Server();
		void	start();
		Server	&operator=(const Server &server);
		std::map<int, ServerConfig>	serverConfigs;
		std::map<int, Socket>		sockets;
		void	closeConnection(epoll_event &event);
		void	createNewClient(Socket &socket);
		bool	isNewClient(const epoll_event &event);
		void	wait();
		void	addFdToPoll(int fd, epoll_event &event);
		void	removeFdFromPoll(int fd, epoll_event &event);
		void	modifyPoll(int fd, epoll_event &event);
		bool	parseConfig(ServerConfig &config);
	private:
		int			_epollfd;
		epoll_event	_events[MAX_EVENTS]; 
};
