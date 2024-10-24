#include "HeaderFields.hpp"
#include "StringUtils.hpp"

std::string	HeaderFields::location(const std::string &url) {
	return "location: " + url;
}

std::string	HeaderFields::contentType(const std::string &str) {
	return "content-type: " + str;
}

std::string	HeaderFields::contentLength(int len) {
	return "content-length: " + StringUtils::itoa(len);
}
