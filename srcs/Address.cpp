#include "Address.hpp"
#include <cstring>
#include <netinet/in.h>

Address::Address(int port, int address) {
	explicit_bzero(&this->addr, sizeof(this->addr));
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.s_addr = address;
}

struct sockaddr &Address::toSockAddr() {
	return reinterpret_cast<struct sockaddr &>(this->addr);
}
