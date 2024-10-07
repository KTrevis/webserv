#include "ConfigParser.hpp"
#include "LocationConfig.hpp"
#include <cwctype>
#include <map>
#include <string>
#include <utility>

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

void	ConfigParser::displayFile() {
	for (size_t i = 0; i < _lines.size(); i++) {
		const std::vector<std::string> &line = _lines[i];
		for (size_t i = 0; i < line.size(); i++)
			std::cout << line[i] << " ";
		std::cout << std::endl;
	}
}

bool	ConfigParser::serverParsing(std::vector<std::string> &line) {
	if (line.size() != 2 || line[1] != "{" || _scope & SERVER) return false;

	_configs.push_back(ServerConfig());
	_scope += SERVER;
	return true;
}

bool	ConfigParser::locationParsing(std::vector<std::string> &line) {
	if (line.size() != 3) return false;

	const std::string &locationName = line[1];
	if (isToken(locationName[0])) return false;

	const std::string &curlyBrace = line[2];
	if (curlyBrace != "{") return false;
	return true;
}

bool	ConfigParser::parseLine(std::vector<std::string> &line) {
	_currScope = strToScope(*line.begin());
	switch (_currScope) {
		case (SERVER): return serverParsing(line);
		case (LOCATION): return locationParsing(line);
		case(NONE): return true;
	}
}

void	ConfigParser::addLocationConfig(size_t &i, const std::string &locationName) {
	ServerConfig	&serverConfig = _configs[_configs.size() - 1];
	try {
		LocationConfig locationConfig(i, _lines);
		serverConfig.locations.insert(std::make_pair(locationName, locationConfig));
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}

ConfigParser::ConfigParser(const std::string &filename) {
	tokenizeFile(filename); // this function is in ConfigTokenizer.cpp
	_scope = 0;
	_currScope = NONE;

	displayFile();
	/* for (size_t i = 0; i < _lines.size(); i++) { */
	/* 	if (!parseLine(_lines[i])) */
	/* 		std::cout << "Parsing failed at line " << i + 1 << std::endl; */
	/* 	if (_currScope == LOCATION) */
	/* 		addLocationConfig(i, _lines[i][1]); */
	/* } */
}
