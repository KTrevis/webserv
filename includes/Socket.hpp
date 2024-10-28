#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <poll.h>
#include "Request.hpp"

class Socket {
	public:
		Socket();
		Socket(int fd);
		~Socket();
		int		getFd() const;
		const int	(&getCgiFd() const)[2];
		int		getServerFd() const;
		void	setup(int fd, int serverFd, ServerConfig &config);
		bool	isServer();
		void	createPipe();
		Request	request;
	private:
		int				_fd;
		int				_serverFd;
		int				_cgiFd[2];
};
