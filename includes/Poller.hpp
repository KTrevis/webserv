#pragma once
#include <vector>
#include "sys/poll.h"
#include "Socket.hpp"

class Poller {
	public:
		void	poll();
		void	addSocket(int fd);
	private:
		std::vector<Socket> sockets;
};
