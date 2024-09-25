#pragma once

#include "Socket.hpp"
#include "Address.hpp"
#include "Epoll.hpp"

class Server {
	public:
		Server(int port);
		void	start();
		int		getFd();
	private:
		Socket	_socket;
		Address	_address;
		Epoll	_epoll;
};
