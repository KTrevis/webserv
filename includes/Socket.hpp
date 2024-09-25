#pragma once
#include <netinet/in.h>
#include <sys/socket.h>
#include <poll.h>

class Socket {
	public:
		Socket();
		Socket(int fd);
		~Socket();
		int	getFd() const;
	private:
		int	_fd;
};
