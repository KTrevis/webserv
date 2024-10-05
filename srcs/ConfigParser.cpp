/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ketrevis <ketrevis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/05 22:58:34 by ketrevis          #+#    #+#             */
/*   Updated: 2024/10/05 23:58:52 by ketrevis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
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

void	ConfigParser::skipWhiteSpace() {
	while (_file[i] && std::iswspace(_file[i]))
		i++;
}

void	ConfigParser::storeKey() {
	while (_file[i] && !std::iswspace(_file[i]) && !isToken(_file[i])) {
		_key += _file[i];
		i++;
	}
}

void	ConfigParser::storeValue() {
	while (_file[i] && _file[i] != '\n' && !std::iswspace(_file[i]) && !isToken(_file[i])) {
		_value += _file[i];
		i++;
	}
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

bool	ConfigParser::parseKeyValue() {
	std::cout << "[KEY]" << _key << std::endl;
	std::cout << "[VALUE]" << _value << std::endl;
	return true;
}

bool	ConfigParser::parseFile() {
	for (; _file[i]; i++) {
		_value = "";
		_key = "";
		skipWhiteSpace();
		storeKey();
		skipWhiteSpace();
		storeValue();
		if (_value == "" && _key == "") continue;
		if (!parseKeyValue()) return false;
	}
	return true;
}

ConfigParser::ConfigParser(const std::string &filename) {
	std::ifstream	stream(filename.c_str());
	std::string		buffer;
	i = 0;
	_scope = 0;

	while (std::getline(stream, buffer))
		_file += buffer + '\n';
	if (!parseFile()) throw std::runtime_error("Configuration file error.");
	
}
