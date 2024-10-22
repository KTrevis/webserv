#pragma once

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <poll.h>
#include <Request.hpp>

class Socket {
	public:
		Socket();
		Socket(int fd);
		~Socket();
		int		getFd() const;
		int		getServerFd() const;
		void	setup(int fd, int serverFd, ServerConfig &config);
		bool	isServer();
		Request	request;
	private:
		int				_fd;
		int				_serverFd;
};
