#pragma once

#include <string>
#include "LocationConfig.hpp"

class CGI {
	public:
		CGI(const std::string &str, LocationConfig &locationConfig);
		void setCGI();
		void				setArgs(const std::vector<std::string> &arr, size_t &i);
		const std::string	&getScriptPath();
		const std::string	&getArgs();
		const std::string	&getBinPath();
	private:
		std::string	_binPath;
		std::string _scriptPath;
		std::string	_args;
		LocationConfig &_locationConfig;
};
