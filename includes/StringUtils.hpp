#pragma once

#include <iostream>
#include <vector>

namespace StringUtils {
	std::vector<std::string> getVectorFile(const std::string &filename);
	std::string	getFileLines(const std::string &filename);
	std::vector<std::string> getFileChunks(const std::string &filename);

	bool	isPositiveNumber(const std::string &str);
	std::string fileExtensionToType(std::string str);
	size_t		getStrVectorSize(const std::vector<std::string> &arr);
	std::string	itoa(int n);
	void 		lowerStr(std::string &c);
	std::vector<std::string> split(std::string str, const std::string &lim, bool prependLim = false);

	std::string createResponse(int httpCode, size_t contentLength, 
		const std::vector<std::string> &fields = std::vector<std::string>());
	std::string createResponse(int httpCode,
			const std::vector<std::string> &fields = std::vector<std::string>(),
			const std::string &body = "");
};
