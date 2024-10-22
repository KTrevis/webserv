#include "Response.hpp"
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

void	Response::setResponse(ServerConfig &serverConfig) {
	LocationConfig locationConfig = findLocation(serverConfig);
	std::string filePath = setFilepath(locationConfig);
	_contentType = StringUtils::fileExtensionToType(filePath);

	Log::Debug("Fetching file at: " + filePath);
	try {
		_body = StringUtils::getFile(filePath);
	} catch (std::exception &e) {
		Log::Error("Failed to read file at: " + filePath);
	}
	_response = "HTTP/1.1 200 OK\r\n";
	_response += "Content-Type: " + _contentType + "\r\n";
	_response += "Content-Length: " + StringUtils::itoa(_body.size()) + "\r\n\r\n";
	_response += _body;
}

static void	redirect(const std::string &url, int clientFd) {
	std::string response = "HTTP/1.1 301\r\n";
	response += "content-length: 0\r\n";
	response += "location: " + url + "\r\n\r\n";
	dprintf(clientFd, "%s", response.c_str());
}

Response::Response(const Socket &client, Request &request, ServerConfig &serverConfig) {
	_i = 0;
	if (request.path[request.path.size() - 1] != '/') {
		redirect(request.path + "/", client.getFd());
		return;
	}
	_split = StringUtils::split(request.path, "/", true);

	if (request.method == "GET")
		setResponse(serverConfig);
	dprintf(client.getFd(), "%s", _response.c_str());
}
