#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

class Address {
	public:
		Address(int port, int address);
		sockaddr	&toSockAddr();
	private:
		sockaddr_in _addr;
};
