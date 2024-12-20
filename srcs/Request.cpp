#include "Request.hpp"
#include "Log.hpp"
#include <algorithm>
#include "StringUtils.hpp"
#include "Utils.hpp"
#include <fstream>
#include <string>
#include "Socket.hpp"

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

static size_t findValueEnd(const std::string &str) {
	for (int i = str.size() - 1; i >= 0; i--) {
		if (i == 0)
			return i;
		if (str[i] != ' ')
			return i;
	}
	return 0;
}

std::pair<std::string, std::string> Request::parseHeaderline() {
	std::pair<std::string, std::string> pair;
	std::string tmp(request, 0, request.find("\r\n"));
	size_t pos = tmp.find(':');
	
	pair.first = tmp.substr(0, pos);
	StringUtils::lowerStr(pair.first);
	pos++;
	while (tmp[pos] == ' ')
		pos++;
	size_t end = findValueEnd(tmp) - pos + 1;
	pair.second = tmp.substr(pos, end);
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
		if (headerArguments.find(tmp.first) != headerArguments.end())
		{
			resCode = 400;
			state = SEND_RESPONSE;
			return;
		}
		headerArguments[tmp.first] = tmp.second;
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
	if (headerArguments.empty())
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
	uploadPath += '/' + name;
	std::ofstream file(uploadPath.c_str());
	std::cout << uploadPath << std::endl;
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
	if (request.find("Content-Type") == std::string::npos)
		request.erase(0, request.find("\r\n") + 2);
	request.erase(0, request.find("\r\n") + 2);
	createPostOutput(name, request.substr(0, request.find(boundarieKey)));
	request.erase(0, request.find(boundarieKey));
}

void	Request::createBody() {
	std::string boundarieKey(request, 0, request.find("\r\n"));
	size_t pos = request.find_last_of("--");
	std::map<std::string, std::string>::iterator it = headerArguments.find("content-type");
	if (it == headerArguments.end())
	{
		resCode = 200;
		return;
	}
	if (it->second.find("boundary=") == std::string::npos)
	{
		resCode = 200;
		return;
	}
	if ("---" + it->second.substr(it->second.find("boundary=") + 10, it->second.size() - it->second.find("boundary=") - 10) != boundarieKey)
	{
		resCode = 200;
		return;
	}
	while ((pos != (request.find(boundarieKey) + boundarieKey.size() + 1)) && pos != std::string::npos) {
		createOneFile(boundarieKey);
		pos = request.find_last_of("--");
	}
	resCode = 201;
	if (pos == std::string::npos && request.size())
	{
		resCode = 200;
		Log::Error("CACA");
	}
}


bool	Request::hasMalformedHeaders() {
	std::map<std::string, std::string>::iterator it;
	for (it = headerArguments.begin(); it != headerArguments.end(); ++it) {
		Log::Error(it->first);
		Log::Error(it->second);
		if (it->first.empty() || it->second.empty()) {
			return true;
		}
	}
	return false;
}

bool	Request::checkMethods() {
	std::map<std::string, LocationConfig>::iterator it = config.locations.begin();
	while (it != config.locations.end()) {
		if (it->first == "allow_methods")
			break;
		it++;
	}
	std::map<std::string, e_methods> map = StringUtils::getStrToMaskMethod();
	return map.find(method) == map.end();
}

bool	Request::checkHeaderArguments() {
	std::map<std::string, std::string>::iterator it = headerArguments.find("content-length");
	if (it == headerArguments.end()) {
		resCode = 411;
		state = SEND_RESPONSE;
		return true;
	}
	if (!StringUtils::isNumeric(it->second)) {
		resCode = 400;
		state = SEND_RESPONSE;
		return true;
	}
	if (config.maxBodySize != -1 && std::atoi(it->second.c_str()) > config.maxBodySize) {
		resCode = 413;
		state = SEND_RESPONSE;
		return true;
	}
	it = headerArguments.find("content-type");
	if (it == headerArguments.end()) {
		resCode = 400;
		state = SEND_RESPONSE;
		return true;
	}
	return false;
}

void	Request::parseBody() {
	std::map<std::string, std::string>::iterator it = headerArguments.find("content-length");
	if (checkHeaderArguments())
		return;
	if (static_cast<size_t> (std::atoi(it->second.c_str())) != request.size())
		return;
	cgiBody = request;
	state = SEND_RESPONSE; 
}

void	Request::parseRequest(Server &server, Socket &client) {
	switch (state) {
		case (PARSE_METHOD) : 
			if (request.find(" ") != std::string::npos) {
				method = parseMethode(); 
				state = PARSE_PATH;
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
				state = CHECK_ERROR;
			}
			break;
		case (CHECK_ERROR) :
			config = Utils::getServerConfig(server, client);
			if (checkMethods())
				resCode = 405;
			else if (httpVer != "HTTP/1.1")
				resCode = 400;
			if (method == "POST") {
				state = PARSE_BODY;
				break;
			}
			state = SEND_RESPONSE;
			return request.clear();
		case (PARSE_BODY) : parseBody(); break;
		case (SEND_RESPONSE) : return;
		case (IDLE) : return;
	}
}
