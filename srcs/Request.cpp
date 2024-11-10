#include "Request.hpp"
#include "Log.hpp"
#include <algorithm>
#include "StringUtils.hpp"
#include "Utils.hpp"
#include <fstream>

Request::Request() {
	resCode = 0;
	state = IDLE;
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
	std::string tmp(request, 0, request.find("\r\n"));
	
	pair.first = tmp.substr(0, tmp.find(":"));
	StringUtils::lowerStr(pair.first);
	pair.second = tmp.substr(tmp.find(":") + 2, tmp.size());
	request.erase(0, request.find("\r\n") + 2);
	return (pair);
}

void Request::parseHeader() {
	std::pair<std::string, std::string> tmp;

	while (request.size()) {
		if (request.find("\r\n") == 0) {
			isHeaderParse = true;
			break;
		}
		tmp = parseHeaderline();
		headerArguments.insert(tmp);
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

void	Request::clear(bool clearState) {
	method.clear();
	path.clear();
	httpVer.clear();
	request.clear();
	headerArguments.clear();
	body.clear();
	if (clearState)
		state = IDLE;
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

void	Request::createPostOutput(std::string &name, std::string content) {
	std::string uploadPath = findPathConfig();
	std::ofstream file(uploadPath.c_str());
	uploadPath += '/' + name;
	if (file.bad())
		return;
	file << content;
	file.close();
}

std::string	Request::findFilename() {
	std::string name("noname");
	std::string tmp(request, 0, request.find("\r\n"));

	if (tmp.find("filename") != std::string::npos) {
		int firstQuote = tmp.find('"', tmp.find("filename")) + 1;
		name = tmp.substr(firstQuote, tmp.find('"', firstQuote) - firstQuote);
	}
	else if (tmp.find("name")) {
		int firstQuote = tmp.find('"', tmp.find("name")) + 1;
		name = tmp.substr(firstQuote, tmp.find('"', firstQuote) - firstQuote);
	}
	request.erase(0, request.find("\r\n") + 2);
	return (name);
}

void Request::createOneFile(std::string &boundarieKey) {
	request.erase(0, request.find("\r\n") + 2);
	std::string name = findFilename();
	if (request.find("Content-Type") == 0)
		request.erase(0, request.find("\r\n") + 2);
	request.erase(0, request.find("\r\n") + 2);
	createPostOutput(name, request.substr(0, request.find(boundarieKey)));
	request.erase(0, request.find(boundarieKey));
}

void	Request::createBody() {
	std::string boundarieKey(request, 0, request.find("\r\n"));
	while ((request.find_last_of("--") != (request.find(boundarieKey) + boundarieKey.size() + 1)))
		createOneFile(boundarieKey);
}

void	Request::parseBody(Server &server, Socket &client) {
	config = Utils::getServerConfig(server, client);
	std::map<std::string, std::string>::iterator it = headerArguments.find("content-length");
	if (it == headerArguments.end())
		return;
	if (static_cast<size_t> (std::atoi(it->second.c_str())) != request.size())
		return;
	cgiBody = body;
	createBody();
	resCode = 202;
}

bool	Request::checkMethods(std::string method) {
	std::map<std::string, LocationConfig>::iterator it = config.locations.begin();
	while (it != config.locations.end()) {
		if (it->first == "allow_methods")
			break;
		it++;
	}
	std::map<std::string, e_methods> map = StringUtils::getStrToMaskMethod();
	return map.find(method) == map.end();
}

void	Request::parseRequest(Server &server, Socket &client) {
	switch (state) {
		case (PARSE_METHOD) : 
			if (request.find(" ") != std::string::npos) {
				method = parseMethode(); 
				state = PARSE_PATH;
			}
			if (checkMethods(method)) {
				resCode = 405;
				state = SEND_RESPONSE;
			}
			break;
		case (PARSE_PATH) :
			if (request.find(" ") != std::string::npos) {
				path = parsePath(); 
				state = PARSE_VERSION;
			}
			break;
		case (PARSE_VERSION) :
			if (request.find("\r\n") != std::string::npos) {
				httpVer = parseVer();
				state = PARSE_HEADERS;
			}
			break;
		case (PARSE_HEADERS) : 
			if (request.find("\r\n\r\n") != std::string::npos) {
				parseHeader();
				if (method == "POST") {
					state = PARSE_BODY;
				} else {
					state = SEND_RESPONSE;
					return request.clear();
				}
			}
			break;
		case (PARSE_BODY) : parseBody(server, client); break;
		case (SEND_RESPONSE) : return;
		case (IDLE) : return;
	}
}
