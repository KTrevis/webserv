#include "Server.hpp"
#include "EventHandler.hpp"
#include "ConfigParser.hpp"
#include "Log.hpp"
#include "NetworkUtils.hpp"
#include "StringUtils.hpp"
#include <asm-generic/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <signal.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>

void	Server::start() {
	while (1)
		wait();
}

enum PortError {
	UNUSED,
	USED,
	DUPLICATE_SERVER_NAME,
};

static PortError 	getPortStatus(std::vector<ServerConfig> &arr, size_t i) {
	int port = arr[i].address.getPort();
	const std::string &hostname = arr[i].serverName;

	if (i == 0)
		return UNUSED;
	i--;
	while (i > 0) {
		if (arr[i].serverName == hostname)
			return DUPLICATE_SERVER_NAME;
		if (arr[i].address.getPort() == port)
			return USED;
		i--;
	}
	if (arr[i].address.getPort() == port && arr[i].serverName == hostname)
		return DUPLICATE_SERVER_NAME;
	if (arr[i].address.getPort() == port)
		return USED;
	return UNUSED;
}

bool Server::parseConfig(ServerConfig &config) {
	int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (fd == -1) return false;

	Log::Trace(StringUtils::itoa(fd) + " server socket created");
	int n = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));
	serverConfigs[fd] = config;
	sockets[fd].setup(fd, fd, config);
	if (NetworkUtils::bind(sockets[fd], config.address) == false)
		return false;
	Log::Info("Listening on port " + StringUtils::itoa(config.address.getPort()));
	if (listen(fd, 5) == -1)
		return false;
	return true;
}

Server::Server(std::vector<ServerConfig> &arr) {
	explicit_bzero(_events, sizeof(_events));
	_epollfd = epoll_create1(0);
	initializeTimer(_epollfd);

	for (size_t i = 0; i < arr.size(); i++) {
		PortError err = getPortStatus(arr, i);
		switch (err) {
			case (USED): continue;
			case (DUPLICATE_SERVER_NAME): 
			throw std::runtime_error("Server construction: duplicate hostname found");
			case (UNUSED):;
		}
		if (!parseConfig(arr[i]))
			throw std::runtime_error("Server constructor failed");
	}

	std::map<int, Socket>::iterator it = sockets.begin();

	for (;it != sockets.end(); it++) {
		const Socket &socket = it->second;
		epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = socket.getFd();
		addFdToPoll(socket.getFd(), event);
	}
}

void	Server::addFdToPoll(int fd, epoll_event &event) {
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &event))
		Log::Error("Adding socket to poll failed");
}

void	Server::modifyPoll(int fd, epoll_event &event) {
	if (epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &event))
		Log::Error("Adding socket to poll failed");
}

void	Server::removeFdFromPoll(int fd, epoll_event &event) {
	if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, &event))
		Log::Error("Deleting fd from poll failed " + StringUtils::itoa(fd));
}

bool Server::isNewClient(const epoll_event &event) {
	std::map<int, Socket>::iterator it = sockets.find(event.data.fd);
	if (it == sockets.end()) return false;
	if (it->second.isServer()) {
		createNewClient(it->second);
		return true;
	}
	return false;
}

void	Server::closeConnection(epoll_event &event) {
	removeFdFromPoll(event.data.fd, event);
	std::map<int, Response>::iterator it = responses.find(event.data.fd);
	if (it != responses.end())
		responses.erase(it);
	if (sockets.find(event.data.fd) == sockets.end()) {
		Log::Info("client socket closed " + StringUtils::itoa(event.data.fd));
		close(event.data.fd);
	}
	else sockets.erase(event.data.fd);
}

void	Server::createNewClient(Socket &socket) {
	std::map<int, ServerConfig>::iterator it;
	it = serverConfigs.find(socket.getFd());
	if (it == serverConfigs.end())
		return;
	ServerConfig &config = it->second;
	int fd = NetworkUtils::accept(socket, config.address);
	epoll_event event;

	if (fd == -1) {
		Log::Error("Server: Accept failed");
		return;
	}
	sockets[fd].setup(fd, socket.getFd(), config);
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
	event.data.fd = fd;
	addFdToPoll(fd, event);
	Log::Info("New client created with socket " + StringUtils::itoa(fd));
}

void	Server::wait() {
	int nbEvents = epoll_wait(_epollfd, _events, MAX_EVENTS, -1);
	if (nbEvents == -1) {
		Log::Error("Server wait failed");
		return;
	}
	for (int i = 0; i < nbEvents; i++)
		EventHandler::handleEvent(*this, _events[i]);
}

void Server::checkClientTimeouts() {
	time_t now = std::time(NULL);

	std::map<int, Socket>::iterator it = sockets.begin();
	while (it != sockets.end()) {
		Socket &client = it->second;
		double inactiveDuration = std::difftime(now, client.lastActivity);

		if (inactiveDuration > CLIENT_TIMEOUT && !it->second.isServer()) {
			Log::Info("Closing inactive connection: " + StringUtils::itoa(client.getFd()));
			epoll_event event;
			++it;
			std::map<int, Response>::iterator itt = responses.find(client.getFd());
			if (itt != responses.end())
				itt->second.getCGI().killCGI();
			dprintf(client.getFd(), "%s", StringUtils::createResponse(504).c_str());
			event.data.fd = client.getFd();
			closeConnection(event);
		}
		else {
			++it;
		}
	}
}

void Server::initializeTimer(int epollFd) {
	int timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	if (timerFd == -1) {
		Log::Error("Failed to create timerfd");
		return;
	}

	struct itimerspec newValue;
	newValue.it_value.tv_sec = 1;
	newValue.it_value.tv_nsec = 0;
	newValue.it_interval.tv_sec = 1;
	newValue.it_interval.tv_nsec = 0;

	if (timerfd_settime(timerFd, 0, &newValue, NULL) == -1) {
		Log::Error("Failed to set timer");
		close(timerFd);
		return;
	}

	epoll_event timerEvent;
	timerEvent.data.fd = timerFd;
	timerEvent.events = EPOLLIN;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, timerFd, &timerEvent);
	
	_timerFd = timerFd;
}

Server::~Server() {
	close(_epollfd);
}
