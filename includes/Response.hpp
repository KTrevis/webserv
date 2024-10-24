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
		void handlePost(Request &request);
		std::string setFilepath(const LocationConfig &locationConfig);
		LocationConfig findLocation(ServerConfig &config);
		std::string _response;
		std::string	_statusCode;
		std::string	_body;
		std::string	_contentType;
		std::vector<std::string> _split;
		size_t		_i;
};
