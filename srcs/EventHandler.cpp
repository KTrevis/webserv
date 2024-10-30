#include "EventHandler.hpp"
#include "HeaderFields.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "StringUtils.hpp"
#include <cstdio>
#include <ostream>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

static std::string	&getStrToModify(int eventFd, Socket &client, Server &server) {
	std::map<int, Response>::iterator it = server.cgiResponses.find(eventFd);

	if (it == server.cgiResponses.end())
		return client.request.request;
	return it->second.getCGI().body;
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
	std::map<int, Response>::iterator it = server.cgiResponses.find(client.getFd());

	if (it != server.cgiResponses.end())
		return;
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
	std::pair<int, Response> pair(client.getFd(), Response(client, config));
	server.cgiResponses.insert(pair);
	it = server.cgiResponses.find(client.getFd());
	Response &response = it->second;
	response.setup(client);
	if (response.getCGI().getScriptPath() != "") {
		Log::Trace("CGI: " + response.getCGI().getScriptPath());
		event.events += EPOLLOUT;
	} else server.cgiResponses.erase(client.getFd());
	request.clear();
	server.modifyPoll(client.getFd(), event);
}

static bool isCGI(int fd, Server &server) {
	return server.cgiResponses.find(fd) != server.cgiResponses.end();
}

static void displayPipe(Server &server, Socket &socket, Response &response) {
	std::string str = StringUtils::createResponse(200, std::vector<std::string>(), response.getCGI().body);
	dprintf(socket.getFd(), "%s", str.c_str());
	server.cgiResponses.erase(socket.getFd());
}

static void readPipe(Response &response) {
	int fd = response.getCGI().getCgiFd()[0];
	char buffer[1024];
	int n = read(fd, buffer, 1024);
	if (n == -1) {
		response.fileIsRed = true;
		return;
	}
	response.getCGI().body.reserve(n);
	for (int i = 0; i < n; i++)
		response.getCGI().body += buffer[i];
}

void	EventHandler::handleEvent(Server &server, epoll_event &event) {
	/* Log::Event(event.events); */
	if (event.events & EPOLLIN && !isCGI(event.data.fd, server) && server.isNewClient(event))
		return;
	if (event.events & EPOLLIN)
		handleReceivedData(server, event);
	if (event.events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		server.closeConnection(event);
		return;
	}
	if (event.events & EPOLLOUT) {
		Socket &client = server.sockets[event.data.fd];
		std::map<int, Response>::iterator it = server.cgiResponses.find(event.data.fd);

		if (it != server.cgiResponses.end() && it->second.getCGI().isReady()) {
			if ((it->second).fileIsRed == false)
				readPipe(it->second);
			else
				displayPipe(server, client, it->second);
			return;
		}
		client.request.parseRequest();
		if (client.request.isReqGenerated == true)
			sendResponse(server, client, event);
	}
}
