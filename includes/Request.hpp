#pragma once

#include <iostream>
#include <map>
#include <vector>
#include "ServerConfig.hpp"

enum State {
	IDLE,
    PARSE_METHOD,
    PARSE_PATH,
    PARSE_VERSION,
    PARSE_HEADERS,
    PARSE_BODY,
    SEND_RESPONSE
};

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
		std::string	body;
		std::string	cgiBody;
		std::map<std::string, std::string>	headerArguments;
		ServerConfig config;
		void		clear();
		bool		isReqGenerated;
		bool		isHeaderParse;
		State		state;
	private:
		bool		checkMethods(std::string method);
		std::string	parseMethode();
		std::string parsePath();
		std::string parseVer();
		void		parseBody();
		void		createBody();
		std::string	findFilename();
		std::string	findPathConfig();
		void		createPostOutput(std::string &name, std::string content);
		void		createOneFile(std::string &boundarieKey);
		void		parseHeader();
		void		displayArgs();
		std::pair<std::string, std::string>	parseHeaderline();
		int	_i;
};
