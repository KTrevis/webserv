#include "ServerConfig.hpp"
#include <strings.h>

ServerConfig::ServerConfig() {
	locations["/"] = LocationConfig();
	bzero(&address, sizeof(address));
	maxBodySize = -1;
}
