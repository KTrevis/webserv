#pragma once

#include <iostream>
#include <map>
#include "Address.hpp"
#include "LocationConfig.hpp"

class ServerConfig {
	public:
		ServerConfig();
		std::map<std::string, LocationConfig>	locations;
		Address	address;
		size_t	maxBodySize;
		std::string	serverName;
		size_t		position;
};
