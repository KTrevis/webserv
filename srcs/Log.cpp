#include "Log.hpp"
#include <sys/epoll.h>
#include <map>

void	Log::Error(const std::string &str) {
	if (LOG_LEVEL >= ERROR)
		std::cerr << "[ERROR]: " << str << std::endl;
}

void	Log::Info(const std::string &str) {
	if (LOG_LEVEL >= INFO)
		std::cout << "[INFO]: " << str << std::endl;
}

void	Log::Debug(const std::string &str) {
	if (LOG_LEVEL >= DEBUG)
		std::cout << "[DEBUG]: " << str << std::endl;
}

void	Log::Trace(const std::string &str) {
	if (LOG_LEVEL >= TRACE)
		std::cout << "[TRACE]: " << str << std::endl;
}

std::map<EPOLL_EVENTS, std::string> getEventsMap() {
	std::map<EPOLL_EVENTS, std::string> eventsMap;

	eventsMap[EPOLLIN] = "EPOLLIN";
	eventsMap[EPOLLPRI] = "EPOLLPRI";
	eventsMap[EPOLLPRI] = "EPOLLPRI";
	eventsMap[EPOLLOUT] = "EPOLLOUT";
	eventsMap[EPOLLRDNORM] = "EPOLLRDNORM";
	eventsMap[EPOLLRDBAND] = "EPOLLRDBAND";
	eventsMap[EPOLLWRNORM] = "EPOLLWRNORM";
	eventsMap[EPOLLWRBAND] = "EPOLLWRBAND";
	eventsMap[EPOLLMSG] = "EPOLLMSG";
	eventsMap[EPOLLERR] = "EPOLLERR";
	eventsMap[EPOLLHUP] = "EPOLLHUP";
	eventsMap[EPOLLRDHUP] = "EEPOLLRDHUP";
	eventsMap[EPOLLEXCLUSIVE] = "EPOLLEXCLUSIVE";
	eventsMap[EPOLLWAKEUP] = "EPOLLWAKEUP";
	eventsMap[EPOLLONESHOT] = "EPOLLONESHOT";
	return eventsMap;
}

void Log::Event(int maskEvent) {
	std::map<EPOLL_EVENTS, std::string> eventsMap = getEventsMap();
	std::map<EPOLL_EVENTS, std::string>::iterator it = eventsMap.begin();

	for (; it != eventsMap.end(); it++) {
		if (maskEvent & it->first) {
			std::string str = "Event happened: " + it->second;
			Log::Debug(str);
		}
	}
}
