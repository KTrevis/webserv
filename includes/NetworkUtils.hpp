#pragma once

#include "Socket.hpp"
#include "Address.hpp"

namespace NetworkUtils {
	bool	bind(const Socket &socket, Address &address);
	int		accept(const Socket &socket, Address &address);
};
