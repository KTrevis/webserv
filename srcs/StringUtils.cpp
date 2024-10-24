#include "StringUtils.hpp"
#include <fstream>
#include <cctype>
#include <stdexcept>

static int	nbrLen(int n)
{
	int		size = 1;
	long	nb = static_cast<long>(n);

	if (n < 0) {
		nb = -nb;
		size++;
	}
	while (nb >= 10) {
		size++;
		nb /= 10;
	}
	return (size);
}

std::string	StringUtils::itoa(int n)
{
	int		i = nbrLen(n);
	long	nb = static_cast<long>(n);
	std::string	str;
	str.resize(i);

	i--;
	if (n == 0)
		str[i] = '0';
	if (n < 0) {
		nb = -nb;
		str[0] = '-';
	}
	while (i >= 0 && nb > 0) {
		str[i] = nb % 10 + 48;
		nb /= 10;
		i--;
	}
	return (str);
}

bool	StringUtils::isPositiveNumber(const std::string &str) {
	for (int i = 0; str[i]; i++) {
		if (!std::isdigit(str[i]))
			return false;
	}
	return true;
}

std::vector<std::string> StringUtils::split(std::string str, const std::string &lim, bool prependLim) {
	std::vector<std::string> arr;
	std::string word;
	size_t pos = 0;

	while ((pos = str.find(lim)) != std::string::npos) {
		word = str.substr(0, pos);
		if (word != "") {
			if (prependLim)
				word = lim + word;
			arr.push_back(word);
		}
		str.erase(0, pos + lim.length());
	}
	if (str != "") {
		word = lim + str;
		arr.push_back(word);
	}
	return arr;
}

std::vector<std::string> StringUtils::getVectorFile(const std::string &filename) {
	std::ifstream			stream(filename.c_str());
	std::string				buffer;
	std::vector<std::string>	file;

	if (stream.fail() || stream.bad())
		throw std::runtime_error("Failed to read file.");
	while (std::getline(stream, buffer))
		file.push_back(buffer);
	return file;
}

std::string	StringUtils::getFile(const std::string &filename) {
	std::ifstream			stream(filename.c_str());
	std::string				buffer;
	std::string				str;

	if (stream.fail())
		throw std::runtime_error("Failed to read file.");
	while (std::getline(stream, buffer))
		str += buffer;
	return str;

}
