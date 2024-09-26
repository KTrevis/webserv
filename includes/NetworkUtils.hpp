#pragma once

#include "Socket.hpp"
#include "Address.hpp"

namespace NetworkUtils {
	void	makeFdNotBlocking(int fd);
	bool	bind(const Socket &socket, Address &address);
	int	accept(const Socket &socket, Address &address);
};
