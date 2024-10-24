#include "Response.hpp"
#include "HeaderFields.hpp"
#include "Log.hpp"
#include "StringUtils.hpp"
#include <cstdio>
#include <unistd.h>
#include <exception>
#include <sys/stat.h>

LocationConfig Response::findLocation(ServerConfig &config) {
	std::map<std::string, LocationConfig>::iterator it;
	std::string filePath;
	std::string	found = "";
	size_t		foundIndex = 0;

	for (;_i < _split.size(); _i++) {
		filePath += _split[_i];
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
	if (it != config.locations.end())
		return it->second;
	return LocationConfig();
}

static bool	isFolder(const char *name)
{
	struct stat	s_stat;

	return (stat(name, &s_stat) == 0 && S_ISDIR(s_stat.st_mode));
}

std::string Response::setFilepath(const LocationConfig &locationConfig) {
	std::string filePath = locationConfig.root;

	for(;_i < _split.size(); _i++)
		filePath += _split[_i];
	filePath += "/";
	if (isFolder(filePath.c_str()))
		filePath += locationConfig.indexFile;
	if (filePath.find_last_of("/") == filePath.size() - 1)
		filePath.erase(filePath.size() - 1);
	return filePath;
}

void	Response::handleGet(ServerConfig &serverConfig) {
	LocationConfig locationConfig = findLocation(serverConfig);
	std::string filePath = setFilepath(locationConfig);
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
	headerFields.reserve(2);
	headerFields.push_back(HeaderFields::contentType(_contentType));
	_response = StringUtils::createResponse(httpCode, headerFields, _body);
}

void	Response::handlePost(Request &request) {
	_response = StringUtils::createResponse(request.resCode);
}

static void	redirect(const std::string &url, int clientFd) {
	std::vector<std::string> headerFields;
	headerFields.reserve(2);
	headerFields.push_back(HeaderFields::location(url));

	std::string response = StringUtils::createResponse(301, headerFields);
	dprintf(clientFd, "%s", response.c_str());
}

Response::Response(Socket &client, ServerConfig &serverConfig) {
	Request &request = client.request;
	_i = 0;
	if (request.path[request.path.size() - 1] != '/') {
		redirect(request.path + "/", client.getFd());
		return;
	}
	_split = StringUtils::split(request.path, "/", true);

	if (request.method == "GET")
		handleGet(serverConfig);
	else if (request.method == "POST")
		handlePost(request);
	dprintf(client.getFd(), "%s", _response.c_str());
}
