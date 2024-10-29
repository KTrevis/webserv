#include "EventHandler.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>

static std::string	&getStrToModify(int eventFd, Socket &client, Server &server) {
	std::map<int, Response*>::iterator it = server.cgiResponses.find(eventFd);

	if (it == server.cgiResponses.end())
		return client.request.request;
	return it->second->getCGI().body;
}

static void handleReceivedData(Server &server, epoll_event event) {
	char buffer[1024];
	Socket &client = server.sockets[event.data.fd];
	std::string &request = getStrToModify(event.data.fd, client, server);
	int n = recv(event.data.fd, buffer, sizeof(buffer), MSG_NOSIGNAL);

	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT;
	server.modifyPoll(event.data.fd, event);
	if (n == -1)
		return Log::Error("recv failed");
	request.reserve(n);
	for (int i = 0; i < n; i++)
		request += buffer[i];
}

static void	sendResponse(Server &server, Socket &client, epoll_event event) {
	Request &request = client.request;
	ServerConfig &config = server.serverConfigs[client.getServerFd()];
	std::map<int, Response*>::iterator it = server.cgiResponses.find(client.getFd());

	if (it != server.cgiResponses.end())
		return;
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
	Response *response = new Response(client, config);
	response->setup(client);
	if (response->getCGI().getScriptPath() != "") {
		Log::Trace("CGI: " + response->getCGI().getScriptPath());
		server.cgiResponses.insert(std::pair<int, Response*>(client.getFd(), response));
		event.events += EPOLLOUT;
	} else delete response;
	request.clear();
	server.modifyPoll(client.getFd(), event);
}

static bool isCGI(int fd, Server &server) {
	return server.cgiResponses.find(fd) != server.cgiResponses.end();
}

static void displayPipe(Response &response) {
	int fd = response.getCGI().getCgiFd()[0];
	char buffer[1024];
	int n = read(fd, buffer, 1023);
	buffer[n] = 0;
	printf("%s", buffer);
}

void	EventHandler::handleEvent(Server &server, epoll_event &event) {
	Log::Event(event.events);
	if (event.events & EPOLLIN && !isCGI(event.data.fd, server) && server.isNewClient(event))
		return;
	if (event.events & EPOLLIN)
		handleReceivedData(server, event);
	if (event.events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		server.closeConnection(event);
		return;
	}
	if (event.events & EPOLLOUT) {
		std::map<int, Response*>::iterator it = server.cgiResponses.find(event.data.fd);
		Socket &client = server.sockets[event.data.fd];

		if (it != server.cgiResponses.end() && it->second->getCGI().isReady()) {
			displayPipe(*it->second);
			return;
		}
		client.request.parseRequest();
		if (client.request.isReqGenerated == true)
			sendResponse(server, client, event);
	}
}
