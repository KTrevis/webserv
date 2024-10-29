#pragma once

#include <string>
#include "Socket.hpp"
#include "LocationConfig.hpp"

class Server;

class CGI {
	public:
		CGI(const std::string &str, LocationConfig &locationConfig, Socket &client);
		CGI();
		CGI	&operator=(const CGI &copy);
		void setCGI();
		void				setArgs(const std::vector<std::string> &arr, size_t &i);
		void				exec(Server &server);
		void				child();
		void				createPipe();
		const std::string	&getScriptPath() const;
		const std::string	&getArgs() const;
		const std::string	&getBinPath() const;
		const int	(&getCgiFd() const)[2];
		std::string			body;
	private:
		LocationConfig &_locationConfig;
		Socket		&_client;
		std::string	_binPath;
		std::string _scriptPath;
		std::string	_args;
		int	_cgiFd[2];
};
