#pragma once

#include <string>

namespace HeaderFields {
	std::string	contentDisposition(const std::string &str, const std::string &fileName);
	std::string	location(const std::string &url);
	std::string	contentType(const std::string &str);
	std::string	contentLength(const int &i);
};
