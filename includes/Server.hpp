#pragma once

#include "Socket.hpp"
#include <map>
#include <vector>
#include <sys/epoll.h>
#include "ServerConfig.hpp"
#include "Response.hpp"

#define MAX_EVENTS 1000

class Server {
	public:
		Server(std::vector<ServerConfig> &arr);
		~Server();
		void	start();
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
		std::map<int, Response*>	cgiResponses;
	private:
		int			_epollfd;
		epoll_event	_events[MAX_EVENTS]; 
};
