#pragma once

#include <iostream>
#include <map>


enum e_methods {
	GET = 1,
	POST = 2,
	DELETE = 4,
};

class Request
{
	public:
		void	parseRequest();
		Request();
		~Request();
		e_methods	method;
		std::string path;
		std::string httpVer;
		std::string request;
		std::map<std::string, std::string>	headerArguments;
		std::string	body;

	private:
		int	_i;
};
