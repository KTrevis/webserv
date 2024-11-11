#include "Response.hpp"
#include "HeaderFields.hpp"
#include "LocationConfig.hpp"
#include "Log.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "StringUtils.hpp"
#include "Utils.hpp"
#include <cmath>
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

std::string Response::getFilepath() {
	std::string filepath = _locationConfig.root;

	for(;_i < _urlSplit.size(); _i++)
		filepath += _urlSplit[_i];
	if (filepath.find_last_of("/") == filepath.size() - 1)
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
	const std::string &toSend = _body[_chunkToSend];

	if (send(_client.getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT | MSG_NOSIGNAL) != -1)
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

static LocationConfig &findAssociatedPath
	(std::map<std::string, LocationConfig> &locations, const std::string &root) {
	std::map<std::string, LocationConfig>::iterator it = locations.begin();

	while (it != locations.end()) {
		if (it->second.root == root)
			return it->second;
		it++;
	}
	throw std::runtime_error("Failed to find associated path");
	return it->second;
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
	LocationConfig &associated = findAssociatedPath(_serverConfig.locations, _locationConfig.root);
	std::string basepath;

	if (associated.name != "/")
		basepath = associated.name;
	for (size_t i = 0; i < _urlSplit.size(); i++)
		basepath += _urlSplit[i] + "/";
	_response = StringUtils::createDirectoryContent(_locationConfig.root, basepath);
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
	if (request.resCode != 0)
		setErrorPage(request.resCode);
	else if (needRedirection(request)) 
		{}
	else if (isDirectoryList())
		createDirectoryList();
	else if ((method == GET || method == POST) && _cgi.getScriptPath() != "")
		_cgi.exec();
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

Response::Response(Socket &client, ServerConfig &serverConfig):
	_client(client),
	_serverConfig(serverConfig),
	_urlSplit(extractParamsFromUrl()),
	_locationConfig(findLocation()),
	_filepath(getFilepath()),
	_cgi(_filepath, _locationConfig, client, _urlParams),
	_pipeEmpty(false),
	_chunkToSend(0) {}

CGI	&Response::getCGI() {
	return _cgi;
}

void	Response::handleCGI() {
	int fd = getCGI().getCgiFd()[0];
	char buffer[1024];
	int n = 1;
	std::string str;

	while (n) {
		n = read(fd, buffer, 1024);
		if (n == -1 || n == 0)
			break;
		buffer[n] = 0;
		str += buffer;
	}
	std::string res = StringUtils::createResponse(200, std::vector<std::string>(), str);
	_cgi._scriptPath = "";
	_body.push_back(res);
}
