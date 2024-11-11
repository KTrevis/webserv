#pragma once

#include "LocationConfig.hpp"
#include "Request.hpp"
#include "ServerConfig.hpp"
#include "CGI.hpp"
#include "Socket.hpp"
#include <sys/epoll.h>

class Server;

#define MethodNotAllowed "method not allowed"

class Response {
	public:
		Response(Socket &client, ServerConfig &config);
		void	setup();
		void	handleCGI();
		bool	fullySent();
		void	sendChunk();
		CGI		&getCGI();
		void	setErrorPage(int httpCode);
		std::string _response;
	private:
		void	handleGet();
		void	redirect(const std::string &url);
		bool	requestIsDir(Request &request);
		void	handleDelete();
		bool	needRedirection(Request &request);
		bool	isDirectoryList();
		void	createDirectoryList();
		std::vector<std::string>	extractParamsFromUrl();
		std::string		getFilepath();
		LocationConfig &findLocation();
		std::string		setCGI();
		std::map<std::string, std::string>	_urlParams;
		Socket						&_client;
		ServerConfig				&_serverConfig;
		std::vector<std::string>	_urlSplit;
		LocationConfig 				&_locationConfig;
		std::vector<std::string>	_body;
		std::string					_filepath;
		std::string					_contentType;
		CGI							_cgi;
		size_t						_i;
		bool 						_pipeEmpty;
		size_t						_chunkToSend;
};
