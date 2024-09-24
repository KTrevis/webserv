#include "NetworkUtils.hpp"

bool	NetworkUtils::bind(const Socket &socket, Address &address) {
	const struct sockaddr *addr = &address.toSockAddr();
	return ::bind(socket.getFd(), addr, sizeof(*addr)) >= 0;
}

int	NetworkUtils::accept(const Socket &socket, Address &address) {
	struct sockaddr *addr = &address.toSockAddr();
	unsigned int len;
	return ::accept(socket.getFd(), addr, &len);
}
