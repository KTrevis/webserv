#pragma once

#include <sys/epoll.h>

class Server;

#define MAX_EVENTS

class Epoll {
	public:
		Epoll(int fdToMonitor);
		~Epoll();
		void	wait();
	private:
		void				createNewConnection();
		int					_serverSocket;
		int					_epollfd;
		Server				&_server;
		struct epoll_event	_events[MAX_EVENTS];
};
