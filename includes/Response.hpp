#pragma once

#include <iostream>
#include "LocationConfig.hpp"
#include "Request.hpp"
#include "ServerConfig.hpp"
#include "CGI.hpp"
#include "Socket.hpp"

class Server;

class Response {
	public:
		Response(Socket &client, ServerConfig &config);
		Response();
		void	setup(Socket &client);
		bool fileIsRed;
		CGI	&getCGI();
		std::string _response;
	private:
		void handleGet();
		void redirect(const std::string &url);
		void handleDelete();
		std::string getFilepath();
		LocationConfig &findLocation(ServerConfig &config);
		std::string setCGI();
		Socket			&_client;
		ServerConfig	&_serverConfig;
		std::vector<std::string> _urlSplit;
		LocationConfig &_locationConfig;
		std::string	_statusCode;
		std::string	_body;
		std::string	_contentType;
		CGI			_cgi;
		size_t		_i;
};
