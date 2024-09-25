#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

class Address {
	public:
		Address(int port, int address);
		struct sockaddr	&toSockAddr();
	private:
		struct sockaddr_in _addr;
};
