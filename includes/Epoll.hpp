#pragma once

#include <sys/epoll.h>

class Server;

#define MAX_EVENTS 100

class Epoll {
	public:
		Epoll(Server &server);
		~Epoll();
		void	wait();
	private:
		bool		isNewClient(const epoll_event &event);
		void		closeConnection(epoll_event &event);
		void		addFdToPoll(int fd, epoll_event &event);
		void		removeFdFromPoll(int fd, epoll_event &event);
		bool		canReadSocket(epoll_event event);
		void		handleNewConnection();
		void		handleReceivedData(epoll_event &event);
		bool		isLoggedOut(epoll_event &event);

		int			_epollfd;
		Server		&_server;
		epoll_event	_events[MAX_EVENTS];
};
