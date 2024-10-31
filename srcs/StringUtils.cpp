#include "StringUtils.hpp"
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
		word = lim + str;
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
	map[".htm"] = "text/html";
	map[".js"] = "application/javascript";
	map[".json"] = "application/json";
	map[".xml"] = "application/xml";
	map[".gif"] = "image/gif";
	map[".jpeg"] = "image/jpeg";
	map[".jpg"] = "image/jpeg";
	map[".png"] = "image/png";
	map[".svg"] = "image/svg+xml";
	map[".ico"] = "image/x-icon";
	map[".woff"] = "font/woff";
	map[".woff2"] = "font/woff2";
	map[".ttf"] = "font/ttf";
	map[".otf"] = "font/otf";
	map[".eot"] = "application/vnd.ms-fontobject";
	map[".pdf"] = "application/pdf";
	map[".zip"] = "application/zip";
	map[".mp3"] = "audio/mpeg";
	map[".wav"] = "audio/wav";
	map[".mp4"] = "video/mp4";
	map[".webm"] = "video/webm";
	map[".ogg"] = "application/ogg";
	map[".txt"] = "text/plain";
	map[".csv"] = "text/csv";
	map[".xls"] = "application/vnd.ms-excel";
	map[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	map[".doc"] = "application/msword";
	map[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	map[".ppt"] = "application/vnd.ms-powerpoint";
	map[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	map[".tar"] = "application/x-tar";
	map[".gz"] = "application/gzip";
	map[".bz2"] = "application/x-bzip2";
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
		addContentLength(str, notFound.size());
		str += notFound;
	} else {
		addContentLength(str, body.size());
		str += body;
	}
	return str;
}
