#pragma once

#include "Socket.hpp"
#include <map>
#include <vector>
#include <sys/epoll.h>
#include "ServerConfig.hpp"
#include "Response.hpp"
#include <sys/timerfd.h>

#define MAX_EVENTS 1000
#define CLIENT_TIMEOUT 5

class Server {
	public:
		Server(std::vector<ServerConfig> &arr);
		~Server();
		void	start();
		std::map<int, std::map<std::string, ServerConfig> >	serverConfigs;
		std::map<int, Socket>		sockets;
		void	closeConnection(epoll_event &event);
		void	createNewClient(Socket &socket);
		bool	isNewClient(const epoll_event &event);
		void	wait();
		void	addFdToPoll(int fd, epoll_event &event);
		void	removeFdFromPoll(int fd, epoll_event &event);
		void	modifyPoll(int fd, epoll_event &event);
		bool	parseConfig(std::map<std::string, ServerConfig> &map);
		void 	checkClientTimeouts();
		void	initializeTimer(int epollFd);
		std::map<int, Response>	responses;
		int			_timerFd;
	private:
		int			_epollfd;
		void		initServerConfigs(std::vector<ServerConfig> &arr);
		epoll_event	_events[MAX_EVENTS]; 
};
