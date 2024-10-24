#include "Response.hpp"
#include "HeaderFields.hpp"
#include "Log.hpp"
#include "StringUtils.hpp"
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

std::string Response::getFilepath(const LocationConfig &locationConfig) {
	std::string filePath = locationConfig.root;

	for(;_i < _urlSplit.size(); _i++)
		filePath += _urlSplit[_i];
	filePath += "/";
	if (isFolder(filePath.c_str()))
		filePath += locationConfig.indexFile;
	if (filePath.find_last_of("/") == filePath.size() - 1)
		filePath.erase(filePath.size() - 1);
	return filePath;
}

void	Response::handleGet(ServerConfig &serverConfig) {
	LocationConfig &locationConfig = findLocation(serverConfig);
	std::string filePath = getFilepath(locationConfig);
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

void Response::handleDelete(ServerConfig &serverConfig) {
	LocationConfig &locationConfig = findLocation(serverConfig);

	if (locationConfig.uploadPath == "") {
		_response = StringUtils::createResponse(403);
		Log::Trace("Failed to find location");
		return;
	}
	std::string filePath = getFilepath(locationConfig);
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
	headerFields.reserve(2);
	headerFields.push_back(HeaderFields::location(url));

	_response = StringUtils::createResponse(301, headerFields);
}

Response::Response(Socket &client, ServerConfig &serverConfig) {
	Request &request = client.request;
	if (request.path[request.path.size() - 1] != '/') {
		redirect(request.path + "/");
		dprintf(client.getFd(), "%s", _response.c_str());
		return;
	}
	_i = 0;
	_urlSplit = StringUtils::split(request.path, "/", true);

	if (request.method == "GET")
		handleGet(serverConfig);
	else if (request.method == "POST")
		_response = StringUtils::createResponse(request.resCode);
	else if (request.method == "DELETE")
		handleDelete(serverConfig);
	dprintf(client.getFd(), "%s", _response.c_str());
}
