#pragma once

#include <iostream>
#include <map>
#include "Address.hpp"
#include "LocationConfig.hpp"

class ServerConfig {
	public:
		std::map<std::string, LocationConfig>	locations;
		Address	address;
		size_t	maxBodySize;
};
