#pragma once

#include "ServerConfig.hpp"
#include <list>
#include <vector>

enum e_scope {
	NONE = 0,
	SERVER = 1,
	LOCATION = 2,
};

class ConfigParser {
	public:
		ConfigParser(const std::string &filename);
		void	displayFile();
	private:
		std::string	tokenizeFile(std::list<std::string> &file);
		bool	scopeIsDuplicated(); // documented in cpp file
		int		_scope;
		e_scope	_currScope;	
		std::string	_file;
		std::list<std::list<std::string> >	_lines;
		std::list<std::list<std::string> >::iterator	_currLine;
		std::vector<ServerConfig>			_configs;
};
