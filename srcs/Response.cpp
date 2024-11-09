#include "Response.hpp"
#include "HeaderFields.hpp"
#include "LocationConfig.hpp"
#include "Log.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "StringUtils.hpp"
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <cstdio>
#include <stdexcept>
#include <unistd.h>
#include <exception>
#include <sys/stat.h>

LocationConfig &Response::findLocation() {
	std::map<std::string, LocationConfig>::iterator it;
	std::string filepath;
	std::string	found = "";
	long int	foundIndex = -1;

	_i = 0;
	for (;_i < _urlSplit.size(); _i++) {
		filepath += _urlSplit[_i];
		it = _serverConfig.locations.find(filepath);
		if (it != _serverConfig.locations.end()) {
			foundIndex = _i;
			found = it->first;
		}
	}
	_i = foundIndex != 1 ? foundIndex + 1 : 0;
	if (found != "")
		return _serverConfig.locations[found];
	it = _serverConfig.locations.find("/");
	if (it == _serverConfig.locations.end())
		throw std::runtime_error("Could not find location config");
	return it->second;
}

static bool	isFolder(const std::string &name) {
	struct stat	s_stat;

	return (stat(name.c_str(), &s_stat) == 0 && S_ISDIR(s_stat.st_mode));
}

std::string Response::getFilepath() {
	std::string filepath = _locationConfig.root;

	for(;_i < _urlSplit.size(); _i++)
		filepath += _urlSplit[_i];
	if (filepath.find_last_of("/") == filepath.size() - 1)
		filepath.erase(filepath.size() - 1);
	return filepath;
}

bool	Response::fullySent() {
	return _chunkToSend == _body.size();
}

void	Response::handleGet() {
	int httpCode;

	if (isFolder(_filepath))
		_filepath += "/" + _locationConfig.indexFile;
	_contentType = StringUtils::fileExtensionToType(_filepath);
	Log::Trace("Fetching file at: " + _filepath);
	try {
		_body = StringUtils::getFileChunks(_filepath);
		httpCode = 200;
	} catch (std::exception &e) {
		Log::Error("Failed to read file at: " + _filepath);
		httpCode = 404;
	}
	std::vector<std::string> headerFields;
	headerFields.reserve(2);
	headerFields.push_back(HeaderFields::contentType(_contentType));
	headerFields.push_back(HeaderFields::contentDisposition("inline", _filepath.substr(_filepath.find_last_of("/") + 1, _filepath.size() - _filepath.find_last_of("/"))));
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
	if (access(_filepath.c_str(), F_OK)) {
		_response = StringUtils::createResponse(404);
		Log::Trace("File does not exist");
		return;
	}
	Log::Trace("Trying to remove " + _filepath);
	remove(_filepath.c_str());
	_response = StringUtils::createResponse(204);
}

void Response::redirect(const std::string &url) {
	std::vector<std::string> headerFields;
	headerFields.reserve(1);
	headerFields.push_back(HeaderFields::location(url));

	_response = StringUtils::createResponse(301, headerFields);
}

static bool strEndsWith(const std::string &str, char c) {
	if (str.size() == 0)
		return false;
	return (str[str.size() - 1] == c);
}

static void	removeTrailingChar(std::string &str, char c) {
	size_t i = str.size() - 1;

	while (str[i] == c) {
		str.erase(i);
		i--;
	}
}


static LocationConfig &findAssociatedPath
	(std::map<std::string, LocationConfig> &locations, const std::string &root) {
	std::map<std::string, LocationConfig>::iterator it = locations.begin();

	while (it != locations.end()) {
		if (it->second.root == root)
			return it->second;
		it++;
	}
	throw std::runtime_error("Failed to find associated path");
	return it->second;
}

bool	Response::needRedirection(Request &request) {
	if (!isFolder(_filepath)) {
		if (strEndsWith(request.path, '/')) {
			removeTrailingChar(request.path, '/');
			Log::Debug(request.path);
			redirect(request.path);
			return true;
		} else return false;
	}
	else if (!strEndsWith(request.path, '/'))
		redirect(request.path + "/");
	else if (_locationConfig.redirection != "")
		redirect(_locationConfig.redirection);
	else return false;
	return true;
}

void	Response::createDirectoryList() {
	LocationConfig &associated = findAssociatedPath(_serverConfig.locations, _locationConfig.root);
	std::string basepath;

	if (associated.name != "/")
		basepath = associated.name;
	for (size_t i = 0; i < _urlSplit.size(); i++)
		basepath += _urlSplit[i] + "/";
	_response = StringUtils::createDirectoryContent(_locationConfig.root, basepath);
}

bool	Response::isDirectoryList() {
	return _locationConfig.autoIndex && isFolder(_filepath);
}

void	Response::setup() {
	_urlSplit.clear();
	_urlSplit = StringUtils::split(_client.request.path, "/", true);
	Request &request = _client.request;
	std::map<std::string, e_methods> map = StringUtils::getStrToMaskMethod();
	std::map<std::string, e_methods>::iterator it = map.find(request.method);
	e_methods method = it->second;

	_i = 0;
	if (request.resCode != 0) {
		_response = StringUtils::createResponse(request.resCode);
		_cgi._scriptPath = "";
	}
	else if (needRedirection(request)) {}
	else if (isDirectoryList())
		createDirectoryList();
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
	_locationConfig(findLocation()),
	_filepath(getFilepath()),
	_cgi(_filepath, _locationConfig, client),
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
