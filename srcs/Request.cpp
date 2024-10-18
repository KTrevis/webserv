#include "Request.hpp"

Request::Request() {
	_i = 0;
}

Request::~Request() {}

void	Request::parseRequest() {
	std::cout << request << std::endl;
	for (;request[_i]; _i++) {
	}
	if (request.find("\r\n\r\n")) {
		_i = 0;
		request.clear();
	}
}
