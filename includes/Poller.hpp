#pragma once
#include <vector>
#include "sys/poll.h"

class Poller {
	public:
		void	poll();
		void	addSocket(int fd);
	private:
		void	copyVectorToArray(struct pollfd *arr);
		std::vector<struct pollfd> sockets;
};
