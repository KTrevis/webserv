#include "StringUtils.hpp"
#include <map>
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

	if (stream.fail())
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

std::string StringUtils::fileExtensionToType(std::string str) {
	size_t pos = str.find_last_of(".");

	if (pos == std::string::npos)
		return "";
	str.erase(0, pos);
	std::map<std::string, std::string> map;
	map[".css"] = "text/css";
	map[".html"] = "text/html";
	return map[str];
}

void StringUtils::lowerStr(std::string &c) {
	size_t i = 0;

	while (c[i]) {
		c[i] = std::tolower(c[i]);
		i++;
	}
}

static std::string get404Page() {
	std::string error;

	error += "<!DOCTYPE html>";
	error += "<html>";
	error += "<head>";
	error += "<title>Page Not Found</title>";
	error += "</head>";
	error += "<body>";
	error += "<p>404 page not found</p>";
	error += "</body>";
	error += "</html>";
	return error;
}

std::string StringUtils::createResponse(int httpCode,
	const std::vector<std::string> &fields, const std::string &body) {
	std::string str = "HTTP/1.1 " + StringUtils::itoa(httpCode) + "\r\n";
	for (size_t i = 0; i < fields.size(); i++)
		str += fields[i] + "\r\n";
	if (httpCode == 404) {
		const std::string &notFound = get404Page();
		str += "content-length: " + StringUtils::itoa(notFound.size()) + "\r\n\r\n";
		str += notFound;
	} else {
		str += "content-length: " + StringUtils::itoa(body.size()) + "\r\n\r\n";
		str += body;
	}
	return str;
}

std::string StringUtils::createResponse(int httpCode, size_t contentLength) {
	std::string str = "HTTP/1.1 " + StringUtils::itoa(httpCode) + "\r\n";
	str += "content-length: " + StringUtils::itoa(contentLength) + "\r\n";
	str += "content-type: text/html\r\n\r\n";
	return str;
}
