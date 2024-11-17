#include "Utils.hpp"
#include <iostream>
#include <map>
#include <sys/stat.h>
#include "ServerConfig.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "StringUtils.hpp"

static std::string	getDefaultServerName
	(std::map<std::string, ServerConfig> &serverConfigs) {
	std::map<std::string, ServerConfig>::iterator it = serverConfigs.begin();

	for (;it != serverConfigs.end(); it++) {
		if (it->second.position == 1)
			return it->second.serverName;
	}
	return "";
}

ServerConfig &Utils::getServerConfig(Server &server, Socket &client) {
	Request &request = client.request;
	const std::string &hostArg = request.headerArguments["host"];
	std::string hostname;

	if (hostArg == "")
		hostname = hostArg;
	else
		hostname = StringUtils::split(hostArg, ":")[0];
	std::map<std::string, ServerConfig> &serverConfigs = server.serverConfigs[client.getPort()];
	if (serverConfigs.find(hostname) == serverConfigs.end())
		hostname = getDefaultServerName(serverConfigs);
	return serverConfigs[hostname];
}

bool	Utils::isFolder(const std::string &name) {
	struct stat	s_stat;

	return (stat(name.c_str(), &s_stat) == 0 && S_ISDIR(s_stat.st_mode));
}

bool Utils::isFile(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}
