#pragma once

#include <iostream>
#include <map>
#include <vector>


enum e_methods {
	GET = 1,
	POST = 2,
	DELETE = 4,
	UNKNOWN = -1,
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
		e_methods	parseMethode();
		std::string parsePath();
		std::string parseVer();
		void		parseHeader();
		std::pair<std::string, std::string>	parseHeaderline();
	private:
		int	_i;
};
