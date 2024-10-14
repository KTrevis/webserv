#pragma once

#include <iostream>
#include <map>
#include "Address.hpp"
#include "LocationConfig.hpp"
#include "Socket.hpp"

class ServerConfig {
	public:
		std::map<std::string, LocationConfig>	locations;
		Address			address;
};
