#pragma once

#include <string>
#include "LocationConfig.hpp"

class CGI {
	public:
		CGI(const std::string &str, LocationConfig &locationConfig);
		void setCGI();
		void				setArgs(const std::vector<std::string> &arr, size_t &i);
		const std::string	&getCgiPath();
		const std::string	&getArgs();
	private:
		std::string _cgiPath;
		std::string	_args;
		LocationConfig &_locationConfig;
};
