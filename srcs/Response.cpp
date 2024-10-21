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

static std::string	getHtml(std::vector<std::string> split, ServerConfig &serverConfig) {
	size_t i = 0;
	LocationConfig locationConfig = findLocation(serverConfig, split, i);
	std::string filePath = locationConfig.root;
	std::string fileContent;

	for(;i < split.size(); i++)
		filePath += split[i];
	filePath += "/";
	if (isFolder(filePath.c_str()))
		filePath += locationConfig.indexFile;
	if (filePath.find_last_of("/") == filePath.size() - 1)
		filePath.erase(filePath.size() - 1);
	Log::Debug("Fetching file at: " + filePath);
	try {
		fileContent = StringUtils::getFile(filePath);
	} catch (std::exception &e) {
		Log::Error("Failed to read file at: " + filePath);
	}
	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
	response += "Content-Length: " + StringUtils::itoa(fileContent.length()) + "\r\n\r\n";
	response += fileContent;
	return response;
}

Response::Response(const Socket &client, const Request &request, ServerConfig &serverConfig) {
	std::vector<std::string> split = StringUtils::split(request.path, "/", true);

	if (split.size() == 0)
		split.push_back("/");
	std::string response;
	if (request.method == "GET")
		response = getHtml(split, serverConfig);
	dprintf(client.getFd(), "%s", response.c_str());
}
