#include "ConfigParser.hpp"
#include "StringUtils.hpp"
#include "LocationConfig.hpp"
#include "Log.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <cwctype>
#include <exception>
#include <map>
#include <netinet/in.h>
#include <stdexcept>
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

static e_scope	strToScope(const std::string &str) {
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
	_scope += LOCATION;
	return true;
}

static unsigned int getAddress(const std::string &address) {
	unsigned int addr;
	if (inet_pton(AF_INET, address.c_str(), &addr) == 0)
		throw std::runtime_error("Invalid IP: " + address);
	return addr;
}

bool	ConfigParser::parseLine(std::vector<std::string> &line) {
	if (line[0] == "listen") {
		if (!(_scope & SERVER) || line.size() != 2) return false;
		std::vector<std::string> split = StringUtils::split(line[1], ":", false);
		const std::string &port = split[split.size() - 1];
		if (split.size() > 2) return false;
		if (!StringUtils::isPositiveNumber(port)) return false;
		Log::Info("Creating server on port " + port);
		unsigned int addr = split.size() == 2 ? getAddress(split[0]) : INADDR_ANY;
		(_configs.end() - 1)->address = Address(addr, std::atoi(port.c_str()));
		return true;
	} else if (line[0] == "max_body_size") {
		if (!(_scope & SERVER) || line.size() != 2) return false;
		if (!StringUtils::isPositiveNumber(line[1])) return false;
		(_configs.end() - 1)->maxBodySize = std::atoi(line[1].c_str());
		return true;
	}

	_currScope = strToScope(line[0]);
	switch (_currScope) {
		case (SERVER): return serverParsing(line);
		case (LOCATION): return locationParsing(line);
		case (NONE): return (line[0] == "location");
	}
	return (false);
}

bool	ConfigParser::addLocationConfig(size_t &i, const std::string &locationName) {
	ServerConfig	&serverConfig = _configs[_configs.size() - 1];
	try {
		serverConfig.locations.insert(std::make_pair(locationName, LocationConfig(i, _lines, locationName)));
		/* locationConfig.displayData(); */
	} catch (std::exception &e) {
		Log::Error(e.what());
		return false;
	}
	return true;
}

ConfigParser::ConfigParser(const std::string &filename) {
	try {
		tokenizeFile(filename); // this function is in ConfigTokenizer.cpp
	} catch (std::exception &e) {
		throw std::runtime_error("Failed to read config file.");
	}
	_scope = 0;
	_currScope = NONE;

	for (size_t i = 0; i < _lines.size(); i++) {
		while (i < _lines.size() && _lines[i][0] == "}") {
			if (_lines[i].size() != 1)
				throw std::runtime_error("ConfigParser constructor: unexpected token");
			_scope -= _currScope;
			if (_scope < 0)
				throw std::runtime_error("ConfigParser constructor: duplicate location/server");
			_currScope = static_cast<e_scope>(_scope);
			i++;
		}
		if (i < _lines.size() && !parseLine(_lines[i]))
			throw std::runtime_error("ConfigParser constructor: parse line failed");
		if (i < _lines.size() && _currScope == LOCATION && !addLocationConfig(i, _lines[i][1]))
			throw std::runtime_error("ConfigParser constructor: location parsing failed");
	}
}

std::vector<ServerConfig>	&ConfigParser::getConfigs() {
	return _configs;
}
