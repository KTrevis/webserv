#pragma once

#include "Poller.hpp"
#include "Socket.hpp"
#include "Address.hpp"

class Server {
	public:
		Server(int port);
	private:
		Socket	socket;
		Address	address;
		Poller	poller;
};
