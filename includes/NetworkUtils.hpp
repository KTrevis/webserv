#pragma once

#include "Socket.hpp"
#include "Address.hpp"

class NetworkUtils {
	public:
		static bool	bind(const Socket &socket, Address &address);
		static int	accept(const Socket &socket, Address &address);
};
