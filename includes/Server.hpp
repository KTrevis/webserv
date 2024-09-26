#pragma once

#include "Socket.hpp"
#include "Address.hpp"
#include "Epoll.hpp"

class Server {
	public:
		Server(int port);
		void	start();
		const Socket	&getSocket();
		Address		&getAdress();
	private:
		Socket	_socket;
		Address	_address;
		Epoll	_epoll;
};
