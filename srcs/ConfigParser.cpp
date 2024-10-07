#include "ConfigParser.hpp"
#include "Log.hpp"
#include <cwctype>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>

e_scope	strToScope(const std::string &str) {
	std::map<std::string, e_scope>	map;
	map["server"] = SERVER;
	map["location"] = LOCATION;

	std::map<std::string, e_scope>::iterator it = map.find(str);
	if (it == map.end()) return NONE;
	return it->second;
}

static bool	isToken(const char &c) {
	return c == '{' || c == '}' || c == ';';
}

void	skipWhiteSpace(int &i, const std::string &str) {
	while (str[i] && std::iswspace(str[i]))
		i++;
}

// this returns true if a scope just found was inside another one of the same type
// for example, a server declared in a server scope itself:
// server {
// 		server {
// 		}
// }
bool	ConfigParser::scopeIsDuplicated() {
	return _scope & _currScope;
}

std::string	getWord(int &i, const std::string &str) {
	std::string	word;

	if (isToken(str[i])) {
		word += str[i];
		i++;
		return word;
	}
	while (str[i] && !std::iswspace(str[i]) && !isToken(str[i])) {
		word += str[i];
		i++;
	}
	return word;
}

std::list<std::string>	tokenizeLine(const std::string &str) {
	std::list<std::string>	list;

	for (int i = 0; str[i];) {
		skipWhiteSpace(i, str);
		list.push_back(getWord(i, str));
	}
	return list;
}

std::string	ConfigParser::tokenizeFile(std::list<std::string> &file) {
	std::list<std::string>::iterator it = file.begin();

	while (it != file.end()) {
		std::list<std::string> list = tokenizeLine(*it);
		if (list.size() != 0)
			_lines.push_back(list);
		it++;
	}
	return "";
}

static void displayLine(std::list<std::string> line) {
	std::list<std::string>::iterator it = line.begin();

	std::cout << "[LINE]: ";
	for (; it != line.end(); it++)
		std::cout << *it << " ";
	std::cout << std::endl;
}

static void	displayFile(std::list<std::list<std::string> > file) {
	std::list<std::list<std::string> >::iterator it = file.begin();

	for (;it != file.end(); it++)
		displayLine(*it);
}

ConfigParser::ConfigParser(const std::string &filename) {
	std::ifstream	stream(filename.c_str());
	std::string		buffer;
	std::list<std::string>	file;
	_scope = 0;

	while (std::getline(stream, buffer))
		file.push_back(buffer);
	tokenizeFile(file);
	displayFile(_lines);
}
