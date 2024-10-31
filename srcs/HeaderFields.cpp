#include "HeaderFields.hpp"
#include "StringUtils.hpp"

std::string	HeaderFields::location(const std::string &url) {
	return "location: " + url;
}

std::string	HeaderFields::contentType(const std::string &str) {
	return "content-type: " + str;
}

std::string	HeaderFields::contentLength(const int &i) {
	return "content-length: " + StringUtils::itoa(i);
}

std::string	HeaderFields::contentDisposition(const std::string &str, const std::string &fileName) {
	return "content-disposition: " + str + "; filename= " + fileName;
}
