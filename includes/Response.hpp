#pragma once

#include <iostream>
#include "Socket.hpp"
#include "Request.hpp"
#include "ServerConfig.hpp"

class Response {
	public:
		Response(Socket &client, ServerConfig &config);
	private:
		void handleGet(ServerConfig &serverConfig);
		void redirect(const std::string &url);
		void handleDelete(ServerConfig &serverConfig);
		std::string getFilepath(const LocationConfig &locationConfig);
		LocationConfig &findLocation(ServerConfig &config);
		std::string _response;
		std::string	_statusCode;
		std::string	_body;
		std::string	_contentType;
		std::vector<std::string> _urlSplit;
		size_t		_i;
};
