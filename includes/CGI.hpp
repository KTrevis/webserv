#pragma once

#include <string>
#include "Socket.hpp"
#include "LocationConfig.hpp"

class Server;

class CGI {
	public:
		CGI(const std::string &str, LocationConfig &locationConfig, Socket &client,
			const std::map<std::string, std::string> &urlParams);
		CGI();
		~CGI();
		void setCGI();
		void				setArgs(const std::vector<std::string> &arr, size_t &i);
		void				exec();
		void				child(Socket &client);
		void				createPipe();
		const std::string	&getScriptPath() const;
		const std::string	&getArgs() const;
		const std::string	&getBinPath() const;
		bool				isReady();
		void				killCGI();
		std::string			createQueryString();
		const int	(&getCgiFd() const)[2];
		std::string			body;
	private:
		int		_exitCode;
		bool	_ready;
		LocationConfig &_locationConfig;
		Socket		&_client;
		const std::map<std::string, std::string>	_urlParams;
		std::string	_binPath;
		std::string _scriptPath;
		std::string	_args;
		int			_pid;
		int	_cgiFd[2];
		friend class Response;
};
