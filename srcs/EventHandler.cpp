#include "EventHandler.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <cstdio>
#include <unistd.h>

static std::string	&getStrToModify(int eventFd, Socket &client) {
	if (eventFd == client.getCgiFd()[0])
		return client.request.cgiResponse;
	return client.request.request;
}

static void handleReceivedData(Server &server, epoll_event event) {
	char buffer[1024];
	Socket &client = server.sockets[event.data.fd];
	std::string &request = getStrToModify(event.data.fd, client);
	int n = recv(event.data.fd, buffer, sizeof(buffer), MSG_NOSIGNAL);

	if (n == -1)
		return Log::Error("recv failed");
	request.reserve(n);
	for (int i = 0; i < n; i++)
		request += buffer[i];
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT;
	server.modifyPoll(event.data.fd, event);
}

static void	sendResponse(Server &server, Socket &client, epoll_event event) {
	Request &request = client.request;
	ServerConfig &config = server.serverConfigs[client.getServerFd()];

	server.responses.insert(std::pair<int, Response>(client.getFd(), Response(client, config)));
	request.clear();
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
	server.modifyPoll(client.getFd(), event);
	if (client.getCgiFd()[1] != -1)
		server.addFdToPoll(client.getCgiFd()[1], event);
}

void	EventHandler::handleEvent(Server &server, epoll_event &event) {
	Log::Event(event.events);
	if (event.events & EPOLLIN && server.isNewClient(event))
		return;
	if (event.events & EPOLLIN)
		handleReceivedData(server, event);
	if (event.events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		server.closeConnection(event);
		return;
	}
	if (event.events & EPOLLOUT) {
		Socket &client = server.sockets[event.data.fd];
		client.request.parseRequest();
		if (client.request.isReqGenerated == true)
			sendResponse(server, client, event);
	}
}
