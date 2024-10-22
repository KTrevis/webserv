#include "Request.hpp"
#include "Log.hpp"
#include <algorithm>
#include "StringUtils.hpp"

Request::Request() {
	resCode = 0;
	isReqGenerated = false;
	isHeaderParse = false;
	_i = 0;
}

Request::~Request() {}

std::string	Request::parseMethode() {
	size_t find =  request.find(" ");
	std::string tmp(request, 0, find);
	request.erase(0, find + 1);
	return (tmp);
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
	StringUtils::lowerStr(pair.first);
	pair.second = tmp.substr(tmp.find(":") + 2, tmp.size());
	request.erase(0, request.find("\r\n") + 2);
	return (pair);
}

void Request::parseHeader() {
	std::pair<std::string, std::string> tmp;

	while (request.size())
	{
		if (request.find("\r\n") == 0)
		{
			isHeaderParse = true;
			break;
		}
		tmp = parseHeaderline();
		headerArguments.insert(tmp);
	}
	if (method == "GET") {
		std::cout << "quoicoubebou" << std::endl;
		isReqGenerated = true;
		return request.clear();
	}
	request.erase(0, 2);
}

void	Request::displayArgs() {
	std::map<std::string, std::string>::iterator it = headerArguments.begin();

	while (it != headerArguments.end()) {
		std::cout << "[KEY]: " << it->first << std::endl;
		std::cout << "[VALUE]: " << it->second << std::endl;
		it++;
	}
}

void	Request::clear() {
	method.clear();
	path.clear();
	httpVer.clear();
	request.clear();
	headerArguments.clear();
	body.clear();
	isHeaderParse = false;
	isReqGenerated = false;
}

void	Request::parseBody() {
	resCode = 202;
	std::map<std::string, std::string>::iterator it = headerArguments.find("content-length");
	if (it == headerArguments.end())
	{
		return;
	}
	// std::cout << static_cast<size_t> (std::atoi(it->second.c_str())) << std::endl;
	// std::cout << request.size() << std::endl;
	// std::cout << request << std::endl;
	if (static_cast<size_t> (std::atoi(it->second.c_str())) == request.size())
	{
	std::cout << "quoicoubaka" << std::endl;
		isReqGenerated = true;
		return;
	}
}


void	Request::parseRequest() {
	if (isHeaderParse == false) {
		method = parseMethode();
		if (method == "")
			return;
		path = parsePath();
		httpVer = parseVer();
		parseHeader();
	}
	// displayArgs();
	if (method == "POST")
		parseBody();
}
