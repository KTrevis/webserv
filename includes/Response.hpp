#pragma once

#include <iostream>
#include "Socket.hpp"
#include "Request.hpp"
#include "ServerConfig.hpp"

class Response {
	public:
		Response(const Socket &client, const Request &request, ServerConfig &config);
	private:
		std::string	statusCode;
		std::string	body;
		std::string	contentType;
};
