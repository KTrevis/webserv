#include "Log.hpp"
#include "Epoll.hpp"
#include "Server.hpp"
#include <cstdio>
#include <cstring>
#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>
#include "NetworkUtils.hpp"

void	Epoll::addFdToPoll(int fd, epoll_event &event) {
	epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &event);
}

void	Epoll::removeFdFromPoll(int fd, epoll_event &event) {
	epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, &event);
}

Epoll::Epoll(Server &server): _server(server) {
	epoll_event event;

	explicit_bzero(_events, sizeof(_events));
	_epollfd = epoll_create1(0);
	event.events = EPOLLIN | EPOLLOUT;
	event.data.fd = server.getSocket().getFd();
	addFdToPoll(server.getSocket().getFd(), event);
}

bool Epoll::isNewClient(const epoll_event &event) {
	return event.data.fd == _server.getSocket().getFd();
}

static void displayNewClient(int fd) {
	char str[1024];
	sprintf(str, "New client created with socket %d", fd);
	Log::Info(str);
}

static void displayClosedClient(int fd) {
	char str[1024];
	sprintf(str, "Closed client with socket %d", fd);
	Log::Info(str);
}

void	Epoll::closeConnection(epoll_event &event) {
	displayClosedClient(event.data.fd);
	removeFdFromPoll(event.data.fd, event);
}

void	Epoll::handleNewConnection() {
	epoll_event event;
	int fd = NetworkUtils::accept(_server.getSocket(), _server.getAdress());

	event.events = EPOLLIN | EPOLLOUT;
	event.data.fd = fd;
	displayNewClient(fd);
	addFdToPoll(fd, event);
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

static void displayEvent(int maskEvent) {
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

void	Epoll::wait() {
	int nbEvents = epoll_wait(_epollfd, _events, MAX_EVENTS, -1);

	for (int i = 0; i < nbEvents; i++) {
		displayEvent(_events[i].events);
		if (_events[i].events & (EPOLLHUP | EPOLLRDHUP))
			closeConnection(_events[i]);
		if (isNewClient(_events[i]))
			handleNewConnection();
	}
}

Epoll::~Epoll() {
	close(_epollfd);
}
