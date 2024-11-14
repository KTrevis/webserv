#include "Response.hpp"
#include "HeaderFields.hpp"
#include "LocationConfig.hpp"
#include "Log.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "StringUtils.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <cmath>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <cstdio>
#include <stdexcept>
#include <unistd.h>
#include <exception>
#include <sys/stat.h>

LocationConfig &Response::findLocation() {
	std::map<std::string, LocationConfig>::iterator it;
	std::string filepath;
	std::string	found = "";
	long int	foundIndex = -1;

	_i = 0;
	for (;_i < _urlSplit.size(); _i++) {
		filepath += _urlSplit[_i];
		it = _serverConfig.locations.find(filepath);
		if (it != _serverConfig.locations.end()) {
			foundIndex = _i;
			found = it->first;
		}
	}
	_i = foundIndex != 1 ? foundIndex + 1 : 0;
	if (found != "")
		return _serverConfig.locations[found];
	it = _serverConfig.locations.find("/");
	if (it == _serverConfig.locations.end())
		throw std::runtime_error("Could not find location config");
	return it->second;
}

static bool	isFolder(const std::string &name) {
	struct stat	s_stat;

	return (stat(name.c_str(), &s_stat) == 0 && S_ISDIR(s_stat.st_mode));
}

static bool isFile(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

std::string Response::getFilepath() {
	std::string filepath = _locationConfig.root;

	for(;_i < _urlSplit.size(); _i++)
		filepath += _urlSplit[_i];
	size_t pos = filepath.find(_locationConfig.name);
	if (pos == _locationConfig.root.size())
		filepath.erase(pos, _locationConfig.root.size());
	if (filepath[filepath.size() - 1] == '/')
		filepath.erase(filepath.size() - 1);
	return filepath;
}

bool	Response::fullySent() {
	return _chunkToSend == _body.size();
}

void	Response::handleGet() {
	int httpCode;

	if (isFolder(_filepath))
		_filepath += "/" + _locationConfig.indexFile;
	if (!isFile(_filepath))
		setErrorPage(404);
	_contentType = StringUtils::fileExtensionToType(_filepath);
	Log::Trace("Fetching file at: " + _filepath);
	try {
		_body = StringUtils::getFileChunks(_filepath);
		httpCode = 200;
	} catch (std::exception &e) {
		Log::Error("Failed to read file at: " + _filepath);
		httpCode = 404;
	}
	std::vector<std::string> headerFields;
	headerFields.reserve(3);
	if (httpCode >= 300) {
		setErrorPage(httpCode);
		return;
	}
	headerFields.push_back(HeaderFields::contentType(_contentType));
	headerFields.push_back(HeaderFields::contentDisposition("inline", _filepath.substr(_filepath.find_last_of("/") + 1, _filepath.size() - _filepath.find_last_of("/"))));
	if (_body.size() != 0)
		headerFields.push_back(HeaderFields::contentLength(StringUtils::getStrVectorSize(_body)));
	_response = StringUtils::createResponse(httpCode, headerFields);
	_body.insert(_body.begin(), _response);
}

void	Response::sendChunk() {
	if (_chunkToSend >= _body.size()) return;
	const std::string &toSend = _body[_chunkToSend];
	ssize_t n = send(_client.getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
	epoll_event event;
	bzero(&event, sizeof(epoll_event));

	if (n == 0) {
		_server.closeConnection(event);
		return;
	}
	if (n != -1)
		_chunkToSend++;
}

void Response::handleDelete() {
	if (_locationConfig.uploadPath == "") {
		_response = StringUtils::createResponse(403);
		_body.push_back(_response);
		Log::Trace("Failed to find location");
		return;
	}
	if (access(_filepath.c_str(), F_OK)) {
		_response = StringUtils::createResponse(404);
		_body.push_back(_response);
		Log::Trace("File does not exist");
		return;
	}
	Log::Trace("Trying to remove " + _filepath);
	remove(_filepath.c_str());
	_response = StringUtils::createResponse(204);
	_body.push_back(_response);
}

void Response::redirect(const std::string &url) {
	std::vector<std::string> headerFields;
	headerFields.reserve(1);
	headerFields.push_back(HeaderFields::location(url));

	_cgi._scriptPath = "";
	_response = StringUtils::createResponse(301, headerFields);
	_body.push_back(_response);
}

static bool strEndsWith(const std::string &str, char c) {
	if (str.size() == 0)
		return false;
	return (str[str.size() - 1] == c);
}

static void	removeTrailingChar(std::string &str, char c) {
	size_t i = str.size() - 1;

	while (str[i] == c) {
		str.erase(i);
		i--;
	}
}

bool	Response::needRedirection(Request &request) {
	if (!isFolder(_filepath) && strEndsWith(request.path, '/')) {
		removeTrailingChar(request.path, '/');
		redirect(request.path);
		return true;
	}
	else if (isFolder(_filepath) && !strEndsWith(request.path, '/')) {
		redirect(request.path + "/");
		return true;
	}
	else if (_locationConfig.redirection != "") {
		redirect(_locationConfig.redirection);
		return true;
	}
	return false;
}

void	Response::createDirectoryList() {
	std::string basepath;

	for (size_t i = 0; i < _urlSplit.size(); i++)
		basepath += _urlSplit[i];
	size_t pos = basepath.find(_locationConfig.name);
	if (pos == 0)
		basepath.erase(0, pos);
	_response = StringUtils::createDirectoryContent(_filepath, basepath);
	_body.push_back(_response);
}

bool	Response::isDirectoryList() {
	return _locationConfig.autoIndex && isFolder(_filepath);
}

void Response::setErrorPage(int httpCode) {
	std::map<int, std::string> &errorPages = _locationConfig.errorPages;
	std::map<int, std::string>::iterator it = errorPages.find(httpCode);

	if (it != errorPages.end()) {
		try {
			const std::string &body = StringUtils::getFile(it->second);
			_response = StringUtils::createResponse(httpCode, std::vector<std::string>(), body);
		} catch (std::exception &e) {
			std::string itoa = StringUtils::itoa(httpCode);
			Log::Error("Failed to read " + itoa + " error page, using default one");
			_response = StringUtils::createResponse(httpCode);
		}
	} else
		_response = StringUtils::createResponse(httpCode);
	_cgi._scriptPath = "";
	_body.clear();
	_body.push_back(_response);
}

void	Response::setup() {
	_urlSplit.clear();
	_urlSplit = StringUtils::split(_client.request.path, "/", true);
	Request &request = _client.request;
	std::map<std::string, e_methods> map = StringUtils::getStrToMaskMethod();
	std::map<std::string, e_methods>::iterator it = map.find(request.method);
	e_methods method = it->second;

	_i = 0;
	if (!(_locationConfig.methodMask & method))
		setErrorPage(405);
	else if (request.resCode != 0)
		setErrorPage(request.resCode);
	else if (needRedirection(request)) 
		{}
	else if (isDirectoryList())
		createDirectoryList();
	else if ((method == GET || method == POST) && _cgi.getScriptPath() != "") {
		if (!isFile(_cgi.getScriptPath()))
			return setErrorPage(404);
		_cgi.exec();
	}
	else if (method == GET)
		handleGet();
	else if (method == POST)
		_body.push_back(StringUtils::createResponse(request.resCode));
	else if (method == DELETE)
		handleDelete();
}

std::vector<std::string>	Response::extractParamsFromUrl() {
	std::vector<std::string> urlSplit = StringUtils::split(_client.request.path, "/", true);
	if (_urlSplit.size() == 0) return urlSplit;
	std::string &url = urlSplit[_urlSplit.size() - 1];

	size_t questionMark = url.find("?");
	if (questionMark == std::string::npos) return urlSplit;
	std::vector<std::string> params = StringUtils::split(url.substr(questionMark + 1), "&");
	for (size_t i = 0; i < params.size(); i++) {
		size_t pos = params[i].find("=");
		std::string key = params[i].substr(0, pos);
		std::string value;
		if (pos != std::string::npos)
			value = params[i].substr(pos + 1);
		_urlParams[key] = value;
	}
	url.erase(questionMark);
	return urlSplit;
}

Response::Response(Socket &client, ServerConfig &serverConfig, Server &server):
	_client(client),
	_serverConfig(serverConfig),
	_urlSplit(extractParamsFromUrl()),
	_locationConfig(findLocation()),
	_filepath(getFilepath()),
	_cgi(_filepath, _locationConfig, client, _urlParams),
	_pipeEmpty(false),
	_chunkToSend(0),
	_server(server) {}

CGI	&Response::getCGI() {
	return _cgi;
}

static std::vector<std::string> extractHeader(std::string &str) {
	size_t pos = str.find("\r\n\r\n");

	if (pos == std::string::npos) return std::vector<std::string>();
	std::string header = str.substr(0, pos);
	std::vector<std::string> split = StringUtils::split(header, "\r\n");
	str.erase(0, pos + 4);
	return split;
}

void	Response::handleCGI() {
	int fd = getCGI().getCgiFd()[0];
	char buffer[1024];
	int n = 1;
	std::string str;

	int code = _cgi._exitCode == 0 ? 200 : 500;
	while (code == 200 && n) {
		n = read(fd, buffer, 1024);
		if (n == -1 || n == 0)
			break;
		buffer[n] = 0;
		str += buffer;
	}
	std::string res = StringUtils::createResponse(code, extractHeader(str), str);
	_cgi._scriptPath = "";
	_body.push_back(res);
}
