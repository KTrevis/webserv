#include "Log.hpp"
#include <sys/epoll.h>

void	Log::Info(const std::string &str) {
	if (LOG_LEVEL >= INFO)
		std::cout << "[INFO]: " << str << std::endl;
}

void	Log::Debug(const std::string &str) {
	if (LOG_LEVEL >= DEBUG)
		std::cout << "[DEBUG]: " << str << std::endl;
}

void	Log::Error(const std::string &str) {
	if (LOG_LEVEL >= ERROR)
		std::cerr << "[ERROR]: " << str << std::endl;
}

static std::string eventToStr(int event) {
	switch (event) {
		case (EPOLLIN): return "EPOLLIN";
		case (EPOLLPRI): return "EPOLLPRI";
		case (EPOLLOUT): return "EPOLLOUT";
		case (EPOLLRDNORM): return "EPOLLRDNORM";
		case (EPOLLRDBAND): return "EPOLLRDBAND";
		case (EPOLLWRNORM): return "EPOLLWRNORM";
		case (EPOLLWRBAND): return "EPOLLWRBAND";
		case (EPOLLMSG): return "EPOLLMSG";
		case (EPOLLERR): return "EPOLLERR";
		case (EPOLLHUP): return "EPOLLHUP";
		case (EPOLLRDHUP): return "EPOLLRDHUP";
		case (EPOLLEXCLUSIVE): return "EPOLLEXCLUSIVE";
		case (EPOLLWAKEUP): return "EPOLLWAKEUP";
		case (EPOLLONESHOT): return "EPOLLONESHOT";
	}
	return "";
}

void Log::Event(int maskEvent) {
	const int size = 14;
	const EPOLL_EVENTS events[size] = {
		EPOLLIN,
		EPOLLPRI,
		EPOLLOUT,
		EPOLLRDNORM,
		EPOLLRDBAND,
		EPOLLWRNORM,
		EPOLLWRBAND,
		EPOLLMSG,
		EPOLLERR,
		EPOLLHUP,
		EPOLLRDHUP,
		EPOLLEXCLUSIVE,
		EPOLLWAKEUP,
		EPOLLONESHOT,
	};

	for (int i = 0; i < size; i++) {
		if (maskEvent & events[i]) {
			std::string str = "Event happened: " + eventToStr(events[i]);
			Log::Debug(str);
		}
	}
}
