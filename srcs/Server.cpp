#include "Server.hpp"
#include "NetworkUtils.hpp"
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

void	Server::start() {
	while (1)
		_epoll.wait();
}

Server::Server(int port): _address(port, INADDR_ANY), _epoll(*this)  {
	if (NetworkUtils::bind(_socket, _address) == false)
		throw std::runtime_error("Server constructor error: Binding failed.");
	if (_socket.getFd() == -1)
		throw std::runtime_error("Server constructor error: Socket creation failed.");
	listen(_socket.getFd(), 5);
}

const Socket &Server::getSocket() {
	return _socket;
}

Address &Server::getAdress() {
	return _address;
}
