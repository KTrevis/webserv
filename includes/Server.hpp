#pragma once

#include <vector>
#include "Socket.hpp"
#include "Address.hpp"
#include "Epoll.hpp"

class Server {
	public:
		Server(int port);
		void	start();
		void	poll();
	private:
		Socket	socket;
		Address	address;
		Epoll	epoll;
};
