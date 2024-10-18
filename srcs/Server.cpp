#include "Server.hpp"
#include "EventHandler.hpp"
#include "ConfigParser.hpp"
#include "Log.hpp"
#include "NetworkUtils.hpp"
#include "StringUtils.hpp"
#include <cstring>
#include <netinet/in.h>
#include <signal.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

void	Server::start() {
	while (1)
		wait();
}

bool Server::parseConfig(ServerConfig &config) {
	int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	Log::Trace(StringUtils::itoa(fd) + " server socket created");
	if (fd == -1) return false;
	serverConfigs[fd] = config;
	sockets[fd].setup(fd, true);
	if (NetworkUtils::bind(sockets[fd], config.address) == false)
		return false;
	if (listen(fd, 5) == -1)
		return false;
	return true;
}

Server::Server(std::vector<ServerConfig> &arr) {
	explicit_bzero(_events, sizeof(_events));
	_epollfd = epoll_create1(0);

	for (size_t i = 0; i < arr.size(); i++) {
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
		Log::Error("Deleting fd from poll failed");
}

bool Server::isNewClient(const epoll_event &event) {
	std::map<int, Socket>::iterator it = sockets.find(event.data.fd);
	if (it->second.isServer()) {
		createNewClient(it->second);
		return true;
	}
	return false;
}

void	Server::closeConnection(epoll_event &event) {
	Log::Info("Closed client with socket " + StringUtils::itoa(event.data.fd));
	removeFdFromPoll(event.data.fd, event);
	close(event.data.fd);
	sockets.erase(event.data.fd);
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
	sockets[fd].setup(fd);
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

Server::~Server() {
	close(_epollfd);
}
