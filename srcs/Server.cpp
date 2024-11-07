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

void	Server::start() {
	while (1)
		wait();
}

static bool	portAlreadyUsed(std::vector<ServerConfig> &arr, size_t i) {
	int port = arr[i].address.getPort();
	if (i == 0)
		return false;
	i--;

	while (i > 0) {
		std::cout << i << std::endl;
		if (arr[i].address.getPort() == port)
			return true;
		i--;
	}
	if (arr[i].address.getPort() == port)
		return true;
	return false;
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

	for (size_t i = 0; i < arr.size(); i++) {
		if (portAlreadyUsed(arr, i)) continue;
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
	else
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

Server::~Server() {
	close(_epollfd);
}
