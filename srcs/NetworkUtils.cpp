#include "NetworkUtils.hpp"
#include <fcntl.h>

void	NetworkUtils::makeFdNotBlocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
      
    fcntl(fd, F_SETFL, new_option);
}

bool	NetworkUtils::bind(const Socket &socket, Address &address) {
	const sockaddr *addr = &address.toSockAddr();
	return ::bind(socket.getFd(), addr, sizeof(*addr)) >= 0;
}

int	NetworkUtils::accept(const Socket &socket, Address &address) {
	sockaddr *addr = &address.toSockAddr();
	unsigned int len = sizeof(*addr);
	return ::accept(socket.getFd(), addr, &len);
}
