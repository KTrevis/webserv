#include "Request.hpp"

Request::Request() {
	_i = 0;
}

Request::~Request() {}

e_methods Request::parseMethode() {
	size_t find =  request.find(" ");
	std::string tmp(request, 0, find);
	if (tmp == "GET") request.erase(0, find + 1); return GET;
	if (tmp == "POST") request.erase(0, find + 1); return POST;
	if (tmp == "DELETE") request.erase(0, find + 1); return DELETE;
	return UNKNOWN;
}

std::string Request::parsePath() {
	std::string tmp(request, 0, request.find(" "));
	request.erase(0, request.find(" ") + 1);
	return (tmp);
}

std::string Request::parseVer() {
	std::string tmp(request, 0,request.find("\r\n"));
	request.erase(0, request.find("\r\n") + 2);
	return (tmp);
}

std::pair<std::string, std::string> Request::parseHeaderline() {
	std::string tmp(request, 0,request.find("\r\n"));
	std::pair<std::string, std::string> pair;
	pair.first = tmp.substr(0, request.find(":"));
	pair.first = tmp.substr(request.find(":") + 2, request.find("\r\n"));
	request.erase(0, request.find("\r\n") + 2);
	return (pair);
}

void Request::parseHeader() {
	std::pair<std::string, std::string> tmp;

	while (request.size() && request != "\r\n\r\n")
	{
		tmp = parseHeaderline();
		headerArguments.insert(tmp);
	}
	request.clear();
}

void	Request::parseRequest() {
	method = parseMethode();
	if (method == UNKNOWN)
		return;
	path = parsePath();
	httpVer = parseVer();
	parseHeader();
}
