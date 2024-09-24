#include "Server.hpp"
#include "NetworkUtils.hpp"
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

Server::Server(int port): address(port, INADDR_ANY)  {
	if (NetworkUtils::bind(this->socket, this->address) == false)
		throw std::runtime_error("Server constructor error: Binding failed.");
	listen(this->socket.getFd(), 5);
	while (1) {
		
	}
}

