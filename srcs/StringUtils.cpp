#include "StringUtils.hpp"
#include "Log.hpp"
#include <dirent.h>
#include <exception>
#include <map>
#include <fstream>
#include <cctype>
#include <stdexcept>

static int	nbrLen(int n) {
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

std::string	StringUtils::itoa(int n) {
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
		if (prependLim)
			word = lim + str;
		else
			word = str;
		arr.push_back(word);
	}
	return arr;
}

std::vector<std::string> StringUtils::getFileChunks(const std::string &filename) {
	const int SIZE = 100000;
	std::ifstream stream(filename.c_str(), std::ios::binary);
	std::vector<std::string> file;
	std::string buffer(SIZE, 0);

	if (stream.fail())
		throw std::runtime_error("Failed to read file.");
	while (stream.read(&buffer[0], SIZE)) {
		buffer.resize(stream.gcount());
		file.push_back(buffer);
	}
	buffer.resize(stream.gcount());
	if (!buffer.empty())
		file.push_back(buffer);
	return file;
}

std::vector<std::string> StringUtils::getVectorFile(const std::string &filename) {
	std::ifstream				stream(filename.c_str());
	std::string					buffer;
	std::vector<std::string>	file;

	if (stream.fail())
		throw std::runtime_error("Failed to read file.");
	while (std::getline(stream, buffer))
		file.push_back(buffer);
	return file;
}

std::string	StringUtils::getFileLines(const std::string &filename) {
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
	std::map<std::string, std::string>::iterator it = map.find(str);
	if (it != map.end())
		return it->second;
	return "text/plain";
}

void StringUtils::lowerStr(std::string &c) {
	size_t i = 0;

	while (c[i]) {
		c[i] = std::tolower(c[i]);
		i++;
	}
}

size_t	StringUtils::getStrVectorSize(const std::vector<std::string> &arr) {
	size_t len = 0;

	for (size_t i = 0; i < arr.size(); i++)
		len += arr[i].size();
	return len;
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

static void addContentLength(std::string &str, size_t size) {
	if (str.find("content-length") == std::string::npos)
		str += "content-length: " + StringUtils::itoa(size) + "\r\n";
	str += "\r\n";
}

std::string StringUtils::createResponse(int httpCode,
	const std::vector<std::string> &fields, const std::string &body) {
	std::string str = "HTTP/1.1 " + StringUtils::itoa(httpCode) + "\r\n";
	for (size_t i = 0; i < fields.size(); i++)
		str += fields[i] + "\r\n";
	if (httpCode == 404) {
		const std::string &notFound = get404Page();
		str += "content-type: text/html\r\n";
		addContentLength(str, notFound.size());
		str += notFound;
	} else {
		addContentLength(str, body.size());
		str += body;
	}
	return str;
}

typedef std::pair<std::string, std::string> StringPair;

static std::string createHtmlTag(const std::string &tag, const std::string &content, const std::vector<StringPair> &attributes = std::vector<StringPair>()) {
	std::string str;

	str += "<" + tag + "";
	for (size_t i = 0; i < attributes.size(); i++) {
		str += " " + attributes[i].first;
		str += "=\"" + attributes[i].second + "\" ";
	}
	str += ">";
	str += content;
	str += "</" + tag + ">";
	return str;
}

static std::vector<std::string> getDirectoryContent(const std::string &dirPath) {
	std::vector<std::string> arr;
	DIR *dir = opendir(dirPath.c_str());

	if (!dir)
		throw std::runtime_error("Failed to open folder");
	for (struct dirent *ent = readdir(dir); ent != NULL; ent = readdir(dir))
		arr.push_back(ent->d_name);
	closedir(dir);
	return arr;
}

std::string StringUtils::createDirectoryContent(const std::string &dirPath, const std::string &basePath) {
	std::vector<std::string> dirContent;
	std::string str;

	try {
		dirContent = getDirectoryContent(dirPath + "/" + basePath);
	} catch (std::exception &e) {
		return StringUtils::createResponse(404);
	}

	for (size_t i = 0; i < dirContent.size(); i++) {
		StringPair pair("href", basePath + dirContent[i]);
		std::vector<StringPair> arr;
		arr.push_back(pair);
		str += createHtmlTag("a", dirContent[i], arr);
		str += "<br>";
	}
	str = StringUtils::createResponse(200, std::vector<std::string>(), str);
	return str;
}

std::map<std::string, e_methods> StringUtils::getStrToMaskMethod() {
	std::map<std::string, e_methods> map;
	map["GET"] = GET;
	map["POST"] = POST;
	map["DELETE"] = DELETE;
	return map;
}
