#pragma once

#include <iostream>
#include "LocationConfig.hpp"
#include "Socket.hpp"
#include "Request.hpp"
#include "ServerConfig.hpp"
#include "CGI.hpp"

class Response {
	public:
		Response(Socket &client, ServerConfig &config);
	private:
		void handleGet();
		void redirect(const std::string &url);
		void handleDelete();
		std::string getFilepath();
		LocationConfig &findLocation(ServerConfig &config);
		std::vector<std::string> _urlSplit;
		std::string setCGI();
		LocationConfig &_locationConfig;
		std::string _response;
		std::string	_statusCode;
		std::string	_body;
		std::string	_contentType;
		CGI			_cgi;
		size_t		_i;
};
