#include "NetworkUtils.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/socket.h>

bool	NetworkUtils::bind(const Socket &socket, Address &address) {
	const sockaddr *addr = &address.toSockAddr();
	return ::bind(socket.getFd(), addr, sizeof(*addr)) == 0;
}

int	NetworkUtils::accept(const Socket &socket, Address &address) {
	sockaddr *addr = &address.toSockAddr();
	unsigned int len = sizeof(*addr);
	return ::accept4(socket.getFd(), addr, &len, SOCK_NONBLOCK);
}
