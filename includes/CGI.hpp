#pragma once

#include <string>
#include "Socket.hpp"
#include "LocationConfig.hpp"

class CGI {
	public:
		CGI(const std::string &str, LocationConfig &locationConfig, Socket &client);
		CGI();
		CGI	&operator=(const CGI &copy);
		void setCGI();
		void				setArgs(const std::vector<std::string> &arr, size_t &i);
		void				exec();
		void				child();
		const std::string	&getScriptPath();
		const std::string	&getArgs();
		const std::string	&getBinPath();
	private:
		LocationConfig &_locationConfig;
		Socket &_client;
		std::string	_binPath;
		std::string _scriptPath;
		std::string	_args;
};
