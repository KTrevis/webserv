#pragma once

#include <sys/epoll.h>
#include "StringUtils.hpp"
#include "Server.hpp"

namespace EventHandler {
	void	handleEvent(Server &server, epoll_event &event);
};
