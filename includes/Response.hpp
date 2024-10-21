#pragma once

#include <iostream>
#include "Socket.hpp"
#include "Request.hpp"
#include "ServerConfig.hpp"

class Response {
	public:
		Response(const Socket &client, Request &request, ServerConfig &config);
		void setResponse(ServerConfig &serverConfig);
	private:
		std::string _response;
		std::string	_statusCode;
		std::string	_body;
		std::string	_contentType;
		std::vector<std::string> _split;
};
