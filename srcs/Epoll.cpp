#include "Log.hpp"
#include "Epoll.hpp"
#include "Server.hpp"
#include "EventHandler.hpp"
#include "NetworkUtils.hpp"
#include "StringUtils.hpp"
#include <algorithm>
#include <cstdio>
#include <fcntl.h>
#include <signal.h>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

Epoll::Epoll(Server &server, std::vector<ServerConfig> &configs): _server(server) {
	signal(SIGPIPE, SIG_IGN);
	explicit_bzero(_events, sizeof(_events));
	_epollfd = epoll_create1(0);

	for (size_t i = 0; i < configs.size(); i++) {
		if (!server.parseConfig(configs[i]))
			throw std::runtime_error("Server constructor failed");
	}

	std::map<int, Socket>::iterator it = server._sockets.begin();
	for (;it != server._sockets.end(); it++) {
		const Socket &socket = it->second;
		epoll_event event;
		event.events = EPOLLIN | EPOLLET;
		event.data.fd = socket.getFd();
		addFdToPoll(socket.getFd(), event);
	}
}

void	Epoll::addFdToPoll(int fd, epoll_event &event) {
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &event))
		Log::Error("Adding socket to poll failed");
}

void	Epoll::modifyPoll(int fd, epoll_event &event) {
	if (epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &event))
		Log::Error("Adding socket to poll failed");
}

void	Epoll::removeFdFromPoll(int fd, epoll_event &event) {
	if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, &event))
		Log::Error("Deleting fd from poll failed");
}

bool Epoll::isNewClient(const epoll_event &event) {
	std::map<int, Socket>::iterator it = _server._sockets.begin();
	for (;it != _server._sockets.end(); it++) {
		if (it->second.getFd() == event.data.fd) {
			createNewClient(event.data.fd);
			return true;
		}
	}
	return false;
}

void	Epoll::closeConnection(epoll_event &event) {
	removeFdFromPoll(event.data.fd, event);
	close(event.data.fd);
	Log::Info("Closed client with socket " + StringUtils::itoa(event.data.fd));
}

void	Epoll::createNewClient(int serverFd) {
	std::map<int, ServerConfig>::iterator it;
	it = _server._serverConfigs.find(serverFd);
	ServerConfig &config = it->second;
	int fd = NetworkUtils::accept(serverFd, config.address);
	epoll_event event;

	if (fd == -1) {
		Log::Error("Epoll: Accept failed");
		return;
	}
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
	event.data.fd = fd;
	addFdToPoll(fd, event);
	Log::Info("New client created with socket " + StringUtils::itoa(fd));
}

void	Epoll::wait() {
	int nbEvents = epoll_wait(_epollfd, _events, MAX_EVENTS, -1);

	if (nbEvents == -1) {
		Log::Error("Epoll wait failed");
		return;
	}
	for (int i = 0; i < nbEvents; i++)
		EventHandler::handleEvent(*this, _events[i]);
}

Epoll::~Epoll() {
	close(_epollfd);
}
