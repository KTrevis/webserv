#pragma once

#include <vector>
#include "Socket.hpp"
#include "Address.hpp"
#include "Epoll.hpp"

class Server {
	public:
		Server(int port);
		void	start();
	private:
		Socket	_socket;
		Address	_address;
		Epoll	_epoll;
};
