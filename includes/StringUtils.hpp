#pragma once

#include <iostream>
#include <vector>

namespace StringUtils {
	std::string	itoa(int n);
	bool	isPositiveNumber(const std::string &str);
	std::vector<std::string> split(std::string str, const std::string &lim, bool prependLim = false);
	std::vector<std::string> getVectorFile(const std::string &filename);
	std::string	getFile(const std::string &filename);
};
