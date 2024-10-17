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
		void	setup(int fd, bool isServer = false);
		std::string	request;
		bool	isServer();
	private:
		bool	_isServer;
		int		_fd;
};
