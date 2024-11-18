#include "ServerConfig.hpp"
#include <strings.h>

ServerConfig::ServerConfig() {
	bzero(&address, sizeof(address));
	maxBodySize = -1;
}
