#include "ConfigParser.hpp"
#include "Log.hpp"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cwctype>
#include <fstream>

static e_context getKeyContext(const std::string &key) {
	std::map<std::string, e_context> map;
	map["server"] = SERVER;
	map["location"] = LOCATION;
	std::map<std::string, e_context>::iterator it = map.find(key);
	if (it == map.end())
		return NONE;
	return it->second;
}

static void	setKeyValue(int &i, const std::string &line, std::string &key, std::string &value)
{
	for (;line[i] && std::iswspace(line[i]); i++); // skips whitespace
	for (;line[i] && !std::iswspace(line[i]) && line[i] != '{'; i++)
		key += line[i];
	for (;line[i] && std::iswspace(line[i]); i++); // skips whitespace
	for (;line[i] && !std::iswspace(line[i]) && line[i] != '{'; i++)
		value += line[i];
}

static bool isNumber(const std::string &str) {
	for (int i = 0; str[i]; i++) {
		if (!isdigit(str[i]))
			return false;
	}
	return true;
}

void	ConfigParser::contextBehaviour() {
	switch (_context) {
		case (SERVER): return _config.push_back(ServerConfig());
		case (LOCATION): return;
		case (NONE): return;
	}
}

bool	ConfigParser::parseLine(const std::string &line) {
	int i = 0;
	std::string	key;
	std::string value;

	setKeyValue(i, line, key, value);
	if (key == "listen") {
		if (_config.size() == 0 || !isNumber(value)) return false;
		_config[_config.size() - 1].listenPort = std::atoi(value.c_str());
		return true;
	}
	while (line[i]) {
		if (!std::iswspace(line[i]) && line[i] != '{')
			return false;
		i++;
	}
	e_context currContext = getKeyContext(key);
	if (currContext != NONE) {
		_context = currContext;
		contextBehaviour();
	}
	return true;
}

int	ConfigParser::parseFile() {
	for (size_t i = 0; i < _file.size(); i++) {
		if (!parseLine(_file[i])) {
			return i;
		}
	}
	return -1;
}

ConfigParser::ConfigParser(const std::string &filename) {
	std::ifstream	filestream(filename.c_str());
	std::string		buffer;

	while (std::getline(filestream, buffer))
		_file.push_back(buffer);

	int err = parseFile();
	if (err != -1) {
		char buffer[1024];
		sprintf(buffer, "Configuration file error at line: %d", err + 1);
		Log::Error(buffer);
	}
}
