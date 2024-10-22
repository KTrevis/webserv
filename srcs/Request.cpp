#include "Request.hpp"
#include "Log.hpp"
#include <algorithm>
#include "StringUtils.hpp"
#include <fstream>

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

std::string	Request::findPathConfig() {
	std::map<std::string, LocationConfig>::iterator it = config.locations.begin();
	while (it != config.locations.end()) {
		if (it->first == path)
			return it->second.uploadPath;
		it++;
	}
	return config.locations.find("/")->second.uploadPath;
}


void	Request::createPostOutput(std::string &name) {
	std::string uploadPath = findPathConfig();
	std::cout << name << std::endl;
	uploadPath += '/' + name;
	std::cout << uploadPath << std::endl;
	std::ofstream file(uploadPath.c_str());
	if (file.bad())
		return;
	file << request;
	file.close();
}


std::string	Request::findFilename() {
	std::string tmp(request, request.find("filename"), request.find("\r\n") - request.find("filename"));
	int firstQuote = tmp.find('"') + 1;
	std::string name(tmp, firstQuote, tmp.find('"', firstQuote) - firstQuote);
	request.erase(0, request.find("\r\n") + 2);
	return (name);
}

void	Request::createBody() {
	std::string boundarieKey(request, 0, request.find("\r\n"));
	request.erase(0, request.find("\r\n") + 2);
	std::string name = findFilename();
	request.erase(0, request.find("\r\n") + 2);
	request.erase(0, request.find("\r\n") + 2);
	request.erase(request.find(boundarieKey));
	createPostOutput(name);
}

void	Request::parseBody() {
	std::map<std::string, std::string>::iterator it = headerArguments.find("content-length");
	if (it == headerArguments.end())
	{
		return;
	}
	if (static_cast<size_t> (std::atoi(it->second.c_str())) == request.size())
	{
		createBody();
		resCode = 202;
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
