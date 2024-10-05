#pragma once

#include <iostream>

enum e_scope {
	NONE,
	SERVER,
	LOCATION,
};

class ConfigParser {
	public:
		ConfigParser(const std::string &filename);
	private:
		void	parseFile();
		void	skipWhiteSpace(int &i);
		void	storeWord(int &i, std::string &str);
		int		_scope;
		std::string	_file;
};
