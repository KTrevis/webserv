#pragma once

#include <iostream>

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
	private:
		int	_i;
};
