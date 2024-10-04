#pragma once

#include <iostream>
#include <vector>
#include "ServerConfig.hpp"

enum e_context {
	NONE,
	SERVER,
	LOCATION,
};

class ConfigParser {
	public:
		ConfigParser(const std::string &filename);
	private:
		void	contextBehaviour();
		int		parseFile();
		bool	parseLine(const std::string &line);
		std::vector<std::string>	_file;
		std::vector<ServerConfig>	_config;
		e_context					_context;
};
