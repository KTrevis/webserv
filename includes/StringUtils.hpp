#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "Request.hpp"

namespace StringUtils {
	std::vector<std::string> getVectorFile(const std::string &filename);
	std::string	getFile(const std::string &filename);
	std::vector<std::string> getFileChunks(const std::string &filename);

	bool	isPositiveNumber(const std::string &str);
	std::string fileExtensionToType(std::string str);
	size_t		getStrVectorSize(const std::vector<std::string> &arr);
	std::string	itoa(int n);
	void 		lowerStr(std::string &c);
	std::vector<std::string> split(std::string str, const std::string &lim, bool prependLim = false);

	std::string createResponse(int httpCode,
			const std::vector<std::string> &fields = std::vector<std::string>(),
			const std::string &body = "");
	std::string createDirectoryContent(const std::string &dirPath, const std::string &basePath);

	std::map<std::string, e_methods> getStrToMaskMethod();
};
