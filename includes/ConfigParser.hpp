#pragma once

#include "ServerConfig.hpp"
#include <list>
#include <vector>

enum e_scope {
	NONE = 0,
	SERVER = 1,
	LOCATION = 2,
};

typedef std::vector<std::string> StringVector;

// the tokenizing part of this class is in ConfigTokenizer.cpp
// the parsing part is in ConfigParser.cpp
class ConfigParser {
	public:
		ConfigParser(const std::string &filename);
		void	displayFile();
		static bool	isToken(const char &c);
		std::vector<ServerConfig>	&getConfigs();
	private:
		std::string	tokenizeFile(const std::string &filename);
		bool	scopeIsDuplicated();
		bool	serverParsing(const std::vector<std::string> &line);
		bool	locationParsing(const std::vector<std::string> &line);
		bool	parseLine(const std::vector<std::string> &line);
		bool	addLocationConfig(size_t &i, const std::string &locationName);
		int		_scope;
		e_scope	_currScope;	
		std::vector<StringVector>	_lines;
		std::vector<ServerConfig>	_configs;
		bool	isServerConfig();
		bool	handleBodySize(const std::vector<std::string> &line);
		bool	handleListen(const std::vector<std::string> &line);
		bool	handleServerName(const std::vector<std::string> &line);
};
