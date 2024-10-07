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
	private:
		std::string	tokenizeFile(const std::string &filename);
		bool	scopeIsDuplicated();
		bool	serverParsing(std::vector<std::string> &line);
		bool	locationParsing(std::vector<std::string> &line);
		bool	parseLine(std::vector<std::string> &line);
		bool	addLocationConfig(size_t &i, const std::string &locationName);
		int		_scope;
		e_scope	_currScope;	
		std::vector<StringVector>	_lines;
		std::vector<ServerConfig>	_configs;
};
