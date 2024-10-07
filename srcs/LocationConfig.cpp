#include "LocationConfig.hpp"
#include <algorithm>
#include <cstdlib>
#include <stdexcept>

bool	LocationConfig::setMethods(const StringVector &arr) {
	std::map<std::string, e_methods> map;
	map["GET"] = GET;
	map["POST"] = POST;
	map["DELETE"] = DELETE;

	for (size_t i = 0; arr.size(); i++) {
		std::map<std::string, e_methods>::iterator it = map.find(arr[i]);
		if (it == map.end()) return false;
		if (!(methodMask & it->second)) // si le flag de la methode n'est deja applique
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
	if (arr.size() != 3) return false;
	redirection.codeHTTP = std::atoi(arr[1].c_str());
	if (redirection.codeHTTP < 100) return false;
	redirection.toRedirect = arr[2];
	return true;
}

static std::string errorString(const std::string &str) {
	return "LocationConfig constructor: " + str + " parsing failed.";
}

LocationConfig::LocationConfig(size_t &i, const std::vector<StringVector> &lines) {
	for (;i < lines.size(); i++) {
		const StringVector &line = lines[i];
		const std::string &key = line[0];

		if (key == "allow_methods" && !setMethods(line))
			throw std::runtime_error(errorString(key));
		else if (key == "root" && !setRoot(line))
			throw std::runtime_error(errorString(key));
		else if (key == "index" && !setIndex(line))
			throw std::runtime_error(errorString(key));
		else if (key == "cgi_extension" && !setCGI(line))
			throw std::runtime_error(errorString(key));
		else if (key == "upload_path" && !setUploadPath(line))
			throw std::runtime_error(errorString(key));
		else if (key == "return" && !setRedirection(line))
			throw std::runtime_error(errorString(key));
		else if (key != "}")
			throw std::runtime_error(key);
		else if (key == "}") return;
	}

}
