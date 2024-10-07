#pragma once

#include "ServerConfig.hpp"
#include <list>
#include <vector>

enum e_scope {
	NONE = 0,
	SERVER = 1,
	LOCATION = 2,
};

typedef std::list<std::string> StringList;

// the tokenizing part of this class is in ConfigTokenizer.cpp
// the parsing part is in ConfigParser.cpp
class ConfigParser {
	public:
		ConfigParser(const std::string &filename);
		void	displayFile();
	private:
		std::string	tokenizeFile(std::list<std::string> &file);
		bool	scopeIsDuplicated();
		bool	serverParsing(std::list<std::string> &line);
		bool	locationParsing(std::list<std::string> &line);
		bool	parseLine(std::list<std::string> &line);
		int		_scope;
		e_scope	_currScope;	
		std::list<StringList>	_lines;
		std::vector<ServerConfig>			_configs;
};
