#pragma once
#include <netinet/in.h>
#include <sys/socket.h>

class Socket {
	public:
		Socket();
		Socket(int fd);
		~Socket();
		int	getFd() const;
	private:
		int		fd;
};
