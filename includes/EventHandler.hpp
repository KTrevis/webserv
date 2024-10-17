#pragma once

#include <sys/epoll.h>
#include "Server.hpp"

namespace EventHandler {
	void	handleEvent(Server &server, epoll_event &event);
};
