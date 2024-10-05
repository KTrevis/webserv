#pragma once

#include "ServerConfig.hpp"
#include <vector>

enum e_scope {
	NONE = 0,
	SERVER = 1,
	LOCATION = 2,
};

class ConfigParser {
	public:
		ConfigParser(const std::string &filename);
	private:
		void	skipWhiteSpace();
		void	storeKey();
		void	storeValue();
		bool	parseKeyValue();
		bool	parseServer();
		bool	parseFile();
		bool	scopeIsDuplicated(); // documented in cpp file
		int		_scope;
		e_scope	_currScope;	
		std::string	_file;
		std::vector<ServerConfig>	_configs;
		std::string _value;
		std::string _key;
		int	i;
};
