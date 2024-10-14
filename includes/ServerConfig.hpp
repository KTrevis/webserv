#pragma once

#include <iostream>
#include <map>
#include "LocationConfig.hpp"

class ServerConfig {
	public:
		std::map<std::string, LocationConfig>	locations;
		int	listenPort;
};
