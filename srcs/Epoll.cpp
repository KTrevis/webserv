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
	event.events = EPOLLIN;
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
	close(event.data.fd);
}

void	Epoll::handleNewConnection() {
	int fd = NetworkUtils::accept(_server.getSocket(), _server.getAdress());
	epoll_event event;

	event.events = EPOLLIN | EPOLLOUT;
	event.data.fd = fd;
	displayNewClient(fd);
	addFdToPoll(fd, event);
}

void Epoll::handleReceivedData(epoll_event &event) {
	(void)event;
}

bool Epoll::isLoggedOut(epoll_event &event) {
	if (event.events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
		return true;
	if (!(event.events & EPOLLIN))
		return false;
	char str[1024];
	int n = read(event.data.fd, str, 1023);
	return n == 0;
}

void	Epoll::wait() {
	int nbEvents = epoll_wait(_epollfd, _events, MAX_EVENTS, -1);

	for (int i = 0; i < nbEvents; i++) {
		Log::Event(_events[i].events);
		if (isLoggedOut(_events[i])) {
			closeConnection(_events[i]);
			continue;
		}
		if (_events[i].events & EPOLLIN && isNewClient(_events[i]))
			handleNewConnection();
		if (_events[i].events & EPOLLIN)
			handleReceivedData(_events[i]);
		if (_events[i].events & EPOLLOUT)
			dprintf(_events[i].data.fd, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 66\n\n<html><head><title>Basic Page</title></head><body><h1>Hello, World!</body></html>");
	}
}

Epoll::~Epoll() {
	close(_epollfd);
}
