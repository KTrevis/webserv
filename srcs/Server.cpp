#include "Server.hpp"
#include "NetworkUtils.hpp"
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

void	Server::start() {
}

Server::Server(int port): _address(port, INADDR_ANY), _epoll(_socket.getFd())  {
	if (NetworkUtils::bind(_socket, _address) == false)
		throw std::runtime_error("Server constructor error: Binding failed.");
	listen(_socket.getFd(), 5);
	_epoll.wait();
}
