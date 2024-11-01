#include "Response.hpp"
#include <sys/types.h>
#include "HeaderFields.hpp"
#include "Log.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "StringUtils.hpp"
#include "Utils.hpp"
#include <cstdio>
#include <stdexcept>
#include <unistd.h>
#include <exception>
#include <sys/stat.h>

LocationConfig &Response::findLocation(ServerConfig &config) {
	std::map<std::string, LocationConfig>::iterator it;
	std::string filePath;
	std::string	found = "";
	size_t		foundIndex = 0;

	_i = 0;
	for (;_i < _urlSplit.size(); _i++) {
		filePath += _urlSplit[_i];
		it = config.locations.find(filePath);
		if (it != config.locations.end()) {
			foundIndex = _i;
			found = it->first;
		}
	}
	_i = foundIndex;
	if (found != "")
		return config.locations[found];
	it = config.locations.find("/");
	if (it == config.locations.end())
		throw std::runtime_error("Could not find location config");
	return it->second;
}

static bool	isFolder(const char *name) {
	struct stat	s_stat;

	return (stat(name, &s_stat) == 0 && S_ISDIR(s_stat.st_mode));
}

std::string Response::getFilepath() {
	std::string filePath = _locationConfig.root;

	for(;_i < _urlSplit.size(); _i++)
		filePath += _urlSplit[_i];
	filePath += "/";
	if (isFolder(filePath.c_str()))
		filePath += _locationConfig.indexFile;
	if (filePath.find_last_of("/") == filePath.size() - 1)
		filePath.erase(filePath.size() - 1);
	return filePath;
}

bool	Response::fullySent() {
	return _chunkToSend == _body.size();
}

void	Response::handleGet() {
	std::string filePath = getFilepath();
	int httpCode;
	_contentType = StringUtils::fileExtensionToType(filePath);

	Log::Debug("Fetching file at: " + filePath);
	try {
		_body = StringUtils::getFileChunks(filePath);
		httpCode = 200;
	} catch (std::exception &e) {
		Log::Error("Failed to read file at: " + filePath);
		httpCode = 404;
	}
	std::vector<std::string> headerFields;
	headerFields.reserve(2);
	headerFields.push_back(HeaderFields::contentType(_contentType));
	if (_body.size() != 0)
		headerFields.push_back(HeaderFields::contentLength(StringUtils::getStrVectorSize(_body)));
	_response = StringUtils::createResponse(httpCode, headerFields);
}

void	Response::sendChunk() {
	const std::string &toSend = _body[_chunkToSend];
	if (send(_client.getFd(), toSend.c_str(), toSend.size(), 0) != -1)
		_chunkToSend++;
}

void Response::handleDelete() {
	if (_locationConfig.uploadPath == "") {
		_response = StringUtils::createResponse(403);
		Log::Trace("Failed to find location");
		return;
	}
	std::string filePath = getFilepath();
	if (access(filePath.c_str(), F_OK)) {
		_response = StringUtils::createResponse(404);
		Log::Trace("File does not exist");
		return;
	}
	Log::Trace("Trying to remove " + filePath);
	remove(filePath.c_str());
	_response = StringUtils::createResponse(204);
}

void Response::redirect(const std::string &url) {
	std::vector<std::string> headerFields;
	headerFields.reserve(1);
	headerFields.push_back(HeaderFields::location(url));

	_response = StringUtils::createResponse(301, headerFields);
}

void	Response::handleRedirections(const Request &request) {
	if (request.path[request.path.size() - 1] != '/')
		redirect(request.path + "/");
	else if (_locationConfig.redirection != "")
		redirect(_locationConfig.redirection);
	else return;

	dprintf(_client.getFd(), "%s", _response.c_str());
}

static bool methodAllowed(int methodMask, e_methods method) {
	return methodMask & method;
}

void	Response::setup() {
	_urlSplit = StringUtils::split(_client.request.path, "/", true);
	Request &request = _client.request;
	e_methods method = StringUtils::getStrToMaskMethod()[request.method];
	handleRedirections(request);
	_i = 0;

	if (!methodAllowed(_locationConfig.methodMask, method)) {
		_response = StringUtils::createResponse(405);
		_cgi._scriptPath = "";
	}
	else if ((method == GET || method == POST) && _cgi.getScriptPath() != "")
		_cgi.exec();
	else if (method == GET)
		handleGet();
	else if (method == POST)
		_response = StringUtils::createResponse(request.resCode);
	else if (method == DELETE)
		handleDelete();
	dprintf(_client.getFd(), "%s", _response.c_str());
}

Response::Response(Socket &client, ServerConfig &serverConfig):
	_client(client),
	_serverConfig(serverConfig),
	_urlSplit(StringUtils::split(client.request.path, "/", true)),
	_locationConfig(findLocation(serverConfig)),
	_cgi(getFilepath(), _locationConfig, client),
	_pipeEmpty(false),
	_chunkToSend(0) {}

CGI	&Response::getCGI() {
	return _cgi;
}

void	Response::readPipe() {
	int fd = getCGI().getCgiFd()[0];
	char buffer[1024];
	int n = read(fd, buffer, 1024);
	if (n == -1) {
		_pipeEmpty = true;
		return;
	}
	buffer[n] = 0;
	getCGI().body += buffer;
}

void	Response::sendCGI(Server &server, epoll_event &event) {
	std::string str = StringUtils::createResponse(200, std::vector<std::string>(), getCGI().body);
	dprintf(_client.getFd(), "%s", str.c_str());
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
	server.responses.erase(_client.getFd());
}

void	Response::handleCGI(Server &server, epoll_event &event) {
	_pipeEmpty ? sendCGI(server, event) : readPipe();
}
