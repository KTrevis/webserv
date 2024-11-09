#pragma once

#include "Socket.hpp"
#include "Address.hpp"

enum SendError {
	SENT,
	DISCONNECTED,
	NOT_READY,
};

namespace NetworkUtils {
	bool	bind(const Socket &socket, Address &address);
	int		accept(const Socket &socket, Address &address);
};

