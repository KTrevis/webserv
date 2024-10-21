#include "Request.hpp"

Request::Request() {
	_i = 0;
}

Request::~Request() {}

e_methods Request::parseMethode() {
	std::string tmp(request, request.find(" ") - 1);
	if (tmp == "GET") request.erase(0, request.find(" ")); return GET;
	if (tmp == "POST") request.erase(0, request.find(" ")); return POST;
	if (tmp == "DELETE") request.erase(0, request.find(" ")); return DELETE;
	return UNKNOWN;
}

std::string Request::parsePath() {
	std::string tmp(request, request.find(" ") - 1);
	request.erase(0, request.find(" "));
	return (tmp);
}

std::string Request::parseVer() {
	std::string tmp(request, request.find("\r\n") - 1);
	request.erase(0, request.find("\r\n") + 1);
	return (tmp);
}

void	Request::parseRequest() {
	method = parseMethode();
	if (method == UNKNOWN)
		return;
	path = parsePath();
	httpVer = parseVer();
}
