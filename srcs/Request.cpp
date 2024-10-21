#include "Request.hpp"

Request::Request() {
	_i = 0;
}

Request::~Request() {}

std::string	Request::parseMethode() {
	size_t find =  request.find(" ");
	std::string tmp(request, 0, find);
	if (tmp == "GET") request.erase(0, find + 1); return "GET";
	if (tmp == "POST") request.erase(0, find + 1); return "POST";
	if (tmp == "DELETE") request.erase(0, find + 1); return "DELETE";
	return "";
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
	std::pair<std::string, std::string> pair;
	std::string tmp(request, 0,request.find("\r\n"));
	
	pair.first = tmp.substr(0, tmp.find(":"));
	pair.second = tmp.substr(tmp.find(":") + 2, tmp.size());
	request.erase(0, request.find("\r\n") + 2);
	return (pair);
}

void Request::parseHeader() {
	std::pair<std::string, std::string> tmp;

	while (request.size())
	{
		if (request.find("\r\n") == 0)
			break;
		tmp = parseHeaderline();
		headerArguments.insert(tmp);
	}
	request.clear();
}

void	Request::displayArgs() {
	std::map<std::string, std::string>::iterator it = headerArguments.begin();

	while (it != headerArguments.end()) {
		std::cout << "[KEY]: " << it->first << std::endl;
		std::cout << "[VALUE]: " << it->second << std::endl;
		it++;
	}
}

void	Request::parseRequest() {
	method = parseMethode();
	if (method == "")
		return;
	path = parsePath();
	httpVer = parseVer();
	parseHeader();
}
