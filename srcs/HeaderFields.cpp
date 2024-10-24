#include "HeaderFields.hpp"
#include "StringUtils.hpp"

std::string	HeaderFields::location(const std::string &url) {
	return "location: " + url;
}

std::string	HeaderFields::contentType(const std::string &str) {
	return "content-type: " + str;
}
