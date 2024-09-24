#include "Server.hpp"
#include "NetworkUtils.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <iostream>

Server::Server(int port): address(port, 0)  {
	if (NetworkUtils::bind(this->socket, this->address) == false)
		throw std::runtime_error("Server constructor error: Binding failed.");
	listen(this->socket.getFd(), 5);
	int clientFd = NetworkUtils::accept(this->socket, this->address);
	std::cout << clientFd << std::endl;
}

