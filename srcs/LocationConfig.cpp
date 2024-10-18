#include "LocationConfig.hpp"
#include <Log.hpp>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <stdexcept>
#include "Request.hpp"

bool	LocationConfig::setMethods(const StringVector &arr) {
	std::map<std::string, e_methods> map;
	map["GET"] = GET;
	map["POST"] = POST;
	map["DELETE"] = DELETE;

	for (size_t i = 1; i < arr.size(); i++) {
		std::map<std::string, e_methods>::iterator it = map.find(arr[i]);
		if (it == map.end()) return false;
		if (!(methodMask & it->second)) // si le flag de la methode n'est pas deja applique
			methodMask += it->second;
	}
	return true;
}

bool	LocationConfig::setRoot(const StringVector &arr) {
	if (arr.size() != 2) return false;
	root = arr[1];
	return true;
}

bool	LocationConfig::setIndex(const StringVector &arr) {
	if (arr.size() != 2) return false;
	indexFile = arr[1];
	return true;
}

bool	LocationConfig::setCGI(const StringVector &arr) {
	if (arr.size() != 3) return false;
	cgi[arr[1]] = arr[2]; // arr 1 should be the file extension, and arr 2 the cgi path
	return true;
}

bool	LocationConfig::setUploadPath(const StringVector &arr) {
	if (arr.size() != 2) return false;
	uploadPath = arr[1];
	return true;
}

bool	LocationConfig::setRedirection(const StringVector &arr) {
	if (arr.size() != 2) return false;
	redirection = arr[1];
	return true;
}

bool	LocationConfig::setAlias(const StringVector &arr) {
	if (arr.size() != 2) return false;
	alias = arr[1];
	return true;
}

bool	LocationConfig::setAutoIndex(const StringVector &arr) {
	if (arr.size() != 2) return false;
	autoIndex = arr[1] == "on";
	return true;
}

static std::string errorString(const std::string &str) {
	return "LocationConfig constructor: " + str + " parsing failed.";
}

// defines the MemberFunctionPtr alias, which is a pointer
// to a function of the LocationConfig class
typedef bool (LocationConfig::*MemberFunctionPtr)(const StringVector&);

LocationConfig::LocationConfig(size_t &i, const std::vector<StringVector> &lines) {
	std::map<std::string, MemberFunctionPtr> map;
	map["allow_methods"] = &LocationConfig::setMethods;
	map["root"] = &LocationConfig::setRoot;
	map["index"] = &LocationConfig::setIndex;
	map["cgi_extension"] = &LocationConfig::setCGI;
	map["upload_path"] = &LocationConfig::setUploadPath;
	map["redirection"] = &LocationConfig::setRedirection;
	map["alias"] = &LocationConfig::setAlias;
	map["autoindex"] = &LocationConfig::setAutoIndex;

	methodMask = 0;
	i++;
	for (;i < lines.size(); i++) {
		const StringVector &line = lines[i];
		const std::string &key = line[0];
		std::map<std::string, MemberFunctionPtr>::iterator it = map.find(key);

		if (it == map.end())
			throw std::runtime_error("invalid key: " + key);
		if (!(this->*(it->second))(line)) // calls the function associated to the string
			throw std::runtime_error(errorString(key));
		if (lines[i + 1][0] == "}") // if end of the location scope
			return;
	}
	displayData();
}

LocationConfig::LocationConfig() {}

static std::string	methodToStr(int method) {
	std::map<int, std::string>	map;
	map[GET] = "GET";
	map[POST] = "POST";
	map[DELETE] = "DELETE";

	std::map<int, std::string>::iterator it = map.find(method);
	if (it == map.end()) return "";
	return it->second;
}

static void	displayCGI(const std::map<std::string, std::string> &map) {
	std::map<std::string, std::string>::const_iterator it = map.begin();

	for (;it != map.end(); it++) {
		std::cout << "[KEY]: " << it->first;
		std::cout << " [VALUE]: " << it->second << std::endl;
	}
}

void	LocationConfig::displayData() {
	std::cout << "[LOCATION CONFIG DATA]" << std::endl;
	std::cout << "Methods: ";
	for (int i = 1; i != DELETE; i *= 2)
		if (methodMask & i)
			std::cout << methodToStr(i) << " ";
	std::cout << std::endl;
	std::cout << "Root :" << root << std::endl;
	std::cout << "Index file: " << indexFile << std::endl;
	std::cout << "Upload path: " << uploadPath << std::endl;
	std::cout << "CGI: " << std::endl;
	displayCGI(cgi);
	std::cout << "Redirection: " << redirection;
	std::cout << std::endl;
}
