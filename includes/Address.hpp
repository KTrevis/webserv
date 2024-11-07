#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

class Address {
	public:
		Address();
		Address(unsigned int adress, int port);
		sockaddr	&toSockAddr();
		int	getPort() const;
	private:
		sockaddr_in _addr;
		int			_port;
};
