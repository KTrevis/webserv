#include "ConfigParser.hpp"

#include <cwctype>
#include <fstream>
#include <string>
#include <vector>

bool	isToken(const std::string &str) {
	std::vector<std::string> tokenList;

	for (size_t i = 0; i < tokenList.size(); i++)
		if (str == tokenList[i]) return true;
	return false;
}

void	ConfigParser::skipWhiteSpace(int &i) {
	while (_file[i] && std::iswspace(_file[i]))
		i++;
}

void	ConfigParser::storeWord(int &i, std::string &str) {
	while (_file[i] && !std::iswspace(_file[i])) {
		str = _file[i];
		i++;
	}
}

void	ConfigParser::parseFile() {
	std::string key;
	std::string value;

	for (int i = 0; _file[i]; i++) {
		skipWhiteSpace(i);
		storeWord(i, key);
		skipWhiteSpace(i);
		storeWord(i, value);
	}
}

ConfigParser::ConfigParser(const std::string &filename) {
	std::ifstream	stream(filename.c_str());
	std::string		buffer;

	while (std::getline(stream, buffer))
		_file += buffer;
	parseFile();
}
