#include "Log.hpp"
#include "Epoll.hpp"
#include "Server.hpp"
#include "EventHandler.hpp"
#include "NetworkUtils.hpp"
#include <cstdio>
#include <signal.h>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

Epoll::Epoll(Server &server): _server(server) {
	epoll_event event;

	signal(SIGPIPE, SIG_IGN);
	explicit_bzero(_events, sizeof(_events));
	_epollfd = epoll_create1(0);
	event.events = EPOLLIN;
	event.data.fd = server.getSocket().getFd();
	addFdToPoll(server.getSocket().getFd(), event);
}

void	Epoll::addFdToPoll(int fd, epoll_event &event) {
	epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &event);
}

void	Epoll::removeFdFromPoll(int fd, epoll_event &event) {
	epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, &event);
}


bool Epoll::isNewClient(const epoll_event &event) {
	return event.data.fd == _server.getSocket().getFd();
}

void	Epoll::closeConnection(epoll_event &event) {
	removeFdFromPoll(event.data.fd, event);
	close(event.data.fd);
	char str[1024];
	sprintf(str, "Closed client with socket %d", event.data.fd);
	Log::Info(str);
}

void	Epoll::createNewClient() {
	int fd = NetworkUtils::accept(_server.getSocket(), _server.getAdress());
	epoll_event event;

	event.events = EPOLLIN | EPOLLOUT;
	event.data.fd = fd;
	addFdToPoll(fd, event);
	char str[1024];
	sprintf(str, "New client created with socket %d", fd);
	Log::Info(str);
}

void	Epoll::wait() {
	int nbEvents = epoll_wait(_epollfd, _events, MAX_EVENTS, -1);

	for (int i = 0; i < nbEvents; i++)
		EventHandler::handleEvent(*this, _events[i]);
}

Epoll::~Epoll() {
	close(_epollfd);
}
