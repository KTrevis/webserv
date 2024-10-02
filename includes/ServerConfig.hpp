#pragma once

#include <iostream>
#include <map>
#include "LocationConfig.hpp"

class ServerConfig {
	public:
	private:
		std::map<std::string, LocationConfig>	locations;
};
