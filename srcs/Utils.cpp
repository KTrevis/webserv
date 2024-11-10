#include "Utils.hpp"
#include <iostream>
#include <map>
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
	if (serverConfigs.find(hostname) == serverConfigs.end() || hostname == "localhost")
		hostname = getDefaultServerName(serverConfigs);
	return serverConfigs[hostname];
}
