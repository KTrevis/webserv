#pragma once

#include <sys/epoll.h>

class Epoll {
	public:
		Epoll(int fdToMonitor);
		~Epoll();
		void	wait();
	private:
		int epollfd;
		struct epoll_event events;
};
