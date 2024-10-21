#include "Response.hpp"
#include "Log.hpp"
#include "StringUtils.hpp"
#include <cstdio>
#include <unistd.h>
#include <exception>
#include <sys/stat.h>

static LocationConfig findLocation(ServerConfig &config, const std::vector<std::string> &split, size_t &i) {
	std::map<std::string, LocationConfig>::iterator it;
	std::string filePath;
	std::string	found = "";
	size_t		foundIndex = 0;

	for (;i < split.size(); i++) {
		filePath += split[i];
		it = config.locations.find(filePath);
		if (it != config.locations.end()) {
			foundIndex = i;
			found = it->first;
		}
	}
	i = foundIndex;
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

static std::string setFilepath(const LocationConfig &locationConfig, const std::vector<std::string> &split, size_t &i) {
	std::string filePath = locationConfig.root;

	for(;i < split.size(); i++)
		filePath += split[i];
	filePath += "/";
	if (isFolder(filePath.c_str()))
		filePath += locationConfig.indexFile;
	if (filePath.find_last_of("/") == filePath.size() - 1)
		filePath.erase(filePath.size() - 1);
	return filePath;
}

void	Response::setResponse(ServerConfig &serverConfig) {
	size_t i = 0;
	LocationConfig locationConfig = findLocation(serverConfig, _split, i);
	std::string filePath = setFilepath(locationConfig, _split, i);
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

Response::Response(const Socket &client, Request &request, ServerConfig &serverConfig) {
	if (request.path[request.path.size() - 1] != '/')
		request.path += "/";
	_split = StringUtils::split(request.path, "/", true);

	if (request.method == "GET")
		setResponse(serverConfig);
	dprintf(client.getFd(), "%s", _response.c_str());
}
