#pragma once

#include <iostream>
#include <map>
#include <vector>


enum e_methods {
	GET = 1,
	POST = 2,
	DELETE = 4,
	UNKNOWN = 8,
};

class Request
{
	public:
		void	parseRequest();
		Request();
		~Request();
		int			resCode;
		std::string	method;
		std::string path;
		std::string httpVer;
		std::string request;
		std::map<std::string, std::string>	headerArguments;
		std::string	body;
		std::string	parseMethode();
		std::string parsePath();
		std::string parseVer();
		void		parseBody();
		bool		isReqGenerated;
		bool		isHeaderParse;
		void		parseHeader();
		void		displayArgs();
		void		clear();
		std::pair<std::string, std::string>	parseHeaderline();
	private:
		int	_i;
};
