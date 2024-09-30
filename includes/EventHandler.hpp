#pragma once

#include <sys/epoll.h>
#include "Epoll.hpp"

namespace EventHandler {
	void	handleEvent(Epoll &epoll, epoll_event &event);
};
