#include "ConfigParser.hpp"
#include "StringUtils.hpp"
#include <cwctype>
#include <fstream>
#include <functional>
#include <map>
#include <string>

// this returns true if a scope just found was inside another one of the same type
// for example, a server declared in a server scope itself:
// server {
// 		server {
// 		}
// }
bool	ConfigParser::scopeIsDuplicated() {
	return _scope & _currScope;
}

e_scope	strToScope(const std::string &str) {
	std::map<std::string, e_scope>	map;
	map["server"] = SERVER;
	map["location"] = LOCATION;

	std::map<std::string, e_scope>::iterator it = map.find(str);
	if (it == map.end()) return NONE;
	return it->second;
}

static void displayLine(std::list<std::string> &line) {
	std::list<std::string>::iterator it = line.begin();

	std::cout << "[LINE]: ";
	for (;it != line.end(); it++)
		std::cout << *it << " ";
	std::cout << std::endl;
}

void	ConfigParser::displayFile() {
	std::list<std::list<std::string> >::iterator it = _lines.begin();

	for (;it != _lines.end(); it++)
		displayLine(*it);
}

bool	ConfigParser::serverParsing(std::list<std::string> &line) {
	if (line.size() != 2) return false;

	std::list<std::string>::iterator it = line.begin();
	it++; // access to the 2nd element of the list
	if (*it != "{") return false;
	if (_scope & SERVER) return false;

	_configs.push_back(ServerConfig());
	_scope += SERVER;
	return true;
}

bool	ConfigParser::locationParsing(std::list<std::string> &line) {
	(void)line;
	return true;
}

bool	ConfigParser::parseLine(std::list<std::string> &line) {
	_currScope = strToScope(*line.begin());
	switch (_currScope) {
		case (SERVER): return serverParsing(line);
		case (LOCATION): return locationParsing(line);
		case(NONE): return true;
	}
	return true;
}

ConfigParser::ConfigParser(const std::string &filename) {
	std::ifstream	stream(filename.c_str());
	std::string		buffer;
	std::list<std::string>	file;
	_scope = 0;
	_currScope = NONE;

	while (std::getline(stream, buffer))
		file.push_back(buffer);
	tokenizeFile(file);
	std::list<StringList>::iterator it = _lines.begin();
	for (;it != _lines.end(); it++) {
		const int err = parseLine(*it);
		if (err) std::cout << "Parsing failed at line " << err << std::endl;
	}
}
