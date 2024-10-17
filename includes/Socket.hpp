#pragma once

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <poll.h>

class Socket {
	public:
		Socket();
		Socket(int fd);
		~Socket();
		int		getFd() const;
		void	setup(int fd);
		std::string	request;
	private:
		int	_fd;
};
