#include "Response.hpp"
#include "Log.hpp"
#include "StringUtils.hpp"
#include <cstdio>
#include <exception>

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

static std::string	getHtml(std::vector<std::string> split, ServerConfig &serverConfig) {
	size_t i = 0;
	LocationConfig locationConfig = findLocation(serverConfig, split, i);
	std::string filePath = locationConfig.root;
	std::string fileContent;

	for(;i < split.size(); i++)
		filePath += split[i];
	filePath += "/" + locationConfig.indexFile;
	Log::Debug("Fetching file at: " + filePath);
	try {
		fileContent = StringUtils::getFile(filePath);
	} catch (std::exception &e) {
		Log::Error("Failed to read file at: " + filePath);
	}
	std::cout << StringUtils::itoa(fileContent.length()) << std::endl;
	std::cout << fileContent.length() << std::endl;
	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
	response += "Content-Length: " + StringUtils::itoa(fileContent.length()) + "\r\n\r\n";
	response += fileContent;
	return response;
}

Response::Response(const Socket &client, const Request &request, ServerConfig &serverConfig) {
	const std::string &url = "/caca/non"; // hardcoded url, will have to get it from the request later
	std::vector<std::string> split = StringUtils::split(url, "/", true);

	(void)request;
	if (split.size() == 0)
		split.push_back("/");
	std::string response = getHtml(split, serverConfig);
	dprintf(client.getFd(), "%s", response.c_str());
}
