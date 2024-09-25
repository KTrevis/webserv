#include "Address.hpp"
#include <cstring>
#include <netinet/in.h>

Address::Address(int port, int address) {
	explicit_bzero(&_addr, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	_addr.sin_addr.s_addr = address;
}

struct sockaddr &Address::toSockAddr() {
	return reinterpret_cast<struct sockaddr &>(_addr);
}
