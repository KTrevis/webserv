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
	while (1) {
		if (stop)
			return;
		wait();
	}
}

bool Server::parseConfig(std::map<std::string, ServerConfig> &map) {
	int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	if (fd == -1) return false;
	int n = 1;
	int port = map.begin()->second.address.getPort();
	if (port == 0) {
		Log::Error("Invalid port");
		return false;
	}
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));

	Log::Trace(StringUtils::itoa(fd) + " server socket created");
	sockets[fd].setup(fd, fd, port);
	if (NetworkUtils::bind(sockets[fd], map.begin()->second.address) == false)
		return false;
	Log::Info("Listening on port " + StringUtils::itoa(port));
	if (listen(fd, 5) == -1)
		return false;
	return true;
}

void	Server::initServerConfigs(std::vector<ServerConfig> &arr) {
	for (size_t i = 0; i < arr.size(); i++) {
		int port = arr[i].address.getPort();
		std::map<std::string, ServerConfig> &map = serverConfigs[port];
		const std::string &hostname = arr[i].serverName;
		if (map.find(hostname) != map.end())
			throw std::runtime_error("duplicate hostname found: " + hostname);
		ServerConfig &config = map[hostname];
		config = arr[i];
		config.position = map.size();
		if (config.locations.find("/") == config.locations.end())
			config.locations["/"] = LocationConfig();
	}
}

Server::Server(std::vector<ServerConfig> &arr) {
	stop = false;
	explicit_bzero(_events, sizeof(_events));
	_epollfd = epoll_create1(0);
	initializeTimer(_epollfd);
	initServerConfigs(arr);

	for (std::map<int, std::map<std::string, ServerConfig> >::iterator it = serverConfigs.begin();
			it != serverConfigs.end(); it++) {
		if (!parseConfig(it->second))
			throw std::runtime_error("Server constructor failed");
	}

	for (std::map<int, Socket>::iterator it = sockets.begin();
			it != sockets.end(); it++) {
		const Socket &socket = it->second;
		epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = socket.getFd();
		addFdToPoll(socket.getFd(), event);
	}
}

void	Server::addFdToPoll(int fd, epoll_event &event) {
	epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &event);
}

void	Server::modifyPoll(int fd, epoll_event &event) {
	epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &event);
}

void	Server::removeFdFromPoll(int fd, epoll_event &event) {
	epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, &event);
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
	std::map<int, std::map<std::string, ServerConfig> >::iterator it;
	it = serverConfigs.find(socket.getPort());
	if (it == serverConfigs.end())
		return;
	ServerConfig &config = it->second.begin()->second;
	int fd = NetworkUtils::accept(socket, config.address);
	epoll_event event;

	if (fd == -1) {
		Log::Error("Server: Accept failed");
		return;
	}
	sockets[fd].setup(fd, socket.getFd(), socket.getPort());
	event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
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
		Response &response = responses.find(client.getFd())->second;
		if (inactiveDuration > CLIENT_TIMEOUT && !it->second.isServer() && (client.request.state != IDLE || response.getCGI().getScriptPath() != "")) {
			Log::Info("Closing inactive connection: " + StringUtils::itoa(client.getFd()));
			epoll_event event;
			++it;
			std::map<int, Response>::iterator itt = responses.find(client.getFd());
			std::string res;
			if (itt != responses.end())
			{
				itt->second.getCGI().killCGI();
				client.request.resCode = 504;
			}
			else
				client.request.resCode = 408;
			event.data.fd = client.getFd();
			event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT;
			modifyPoll(event.data.fd, event);
			client.request.state = IDLE;
			/* closeConnection(event); */
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
	close(_timerFd);
}
