#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

class Address {
	public:
		Address();
		Address(int adress, int port);
		sockaddr	&toSockAddr();
	private:
		sockaddr_in _addr;
};
