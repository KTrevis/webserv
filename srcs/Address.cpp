#include "Address.hpp"
#include <iostream>
#include <cstring>
#include <netinet/in.h>

Address::Address() {}

Address::Address(unsigned int address, int port) {
	explicit_bzero(&_addr, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	_addr.sin_addr.s_addr = address;
	_port = port;
}

int	Address::getPort() const {
	return _port;
}

sockaddr &Address::toSockAddr() {
	return reinterpret_cast<sockaddr &>(_addr);
}
