#pragma once

#include <sys/epoll.h>
#include "Socket.hpp"
#include "ServerConfig.hpp"

class Server;

#define MAX_EVENTS 100

class Epoll {
	public:
		Epoll(Server &server, std::vector<ServerConfig> &configs);
		~Epoll();
		void	closeConnection(epoll_event &event);
		void	createNewClient(int serverFd);
		bool	isNewClient(const epoll_event &event);
		epoll_event*	getEvents();
		int				getFd();
		void	wait();
		void	addFdToPoll(int fd, epoll_event &event);
		void	removeFdFromPoll(int fd, epoll_event &event);
		void	modifyPoll(int fd, epoll_event &event);
		Server		&_server;
	private:
		int			_epollfd;
		epoll_event	_events[MAX_EVENTS]; 
};
