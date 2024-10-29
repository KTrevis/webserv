#include "Response.hpp"
#include "HeaderFields.hpp"
#include "Log.hpp"
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

static bool	isFolder(const char *name)
{
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

void	Response::handleGet() {
	std::string filePath = getFilepath();
	int httpCode;
	_contentType = StringUtils::fileExtensionToType(filePath);

	Log::Debug("Fetching file at: " + filePath);
	try {
		_body = StringUtils::getFile(filePath);
		httpCode = 200;
	} catch (std::exception &e) {
		Log::Error("Failed to read file at: " + filePath);
		httpCode = 404;
	}
	std::vector<std::string> headerFields;
	headerFields.reserve(1);
	headerFields.push_back(HeaderFields::contentType(_contentType));
	_response = StringUtils::createResponse(httpCode, headerFields, _body);
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

void	Response::setup(Socket &client) {
	_urlSplit = StringUtils::split(_client.request.path, "/", true);
	Request &request = _client.request;

	if (request.path[request.path.size() - 1] != '/') {
		redirect(request.path + "/");
		dprintf(_client.getFd(), "%s", _response.c_str());
		return;
	}
	_i = 0;

	if ((request.method == "GET" || request.method == "POST") && _cgi.getScriptPath() != "")
		_cgi.exec(client);
	else if (request.method == "GET")
		handleGet();
	else if (request.method == "POST")
		_response = StringUtils::createResponse(request.resCode);
	else if (request.method == "DELETE")
		handleDelete();
	dprintf(_client.getFd(), "%s", _response.c_str());
}

Response::Response(Socket &client, ServerConfig &serverConfig):
	_client(client),
	_serverConfig(serverConfig),
	_urlSplit(StringUtils::split(client.request.path, "/", true)),
	_locationConfig(findLocation(serverConfig)),
	_cgi(getFilepath(), _locationConfig, client) {}

CGI	&Response::getCGI() {
	return _cgi;
}
