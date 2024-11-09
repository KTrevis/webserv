#include "StringUtils.hpp"
#include "Log.hpp"
#include <dirent.h>
#include <exception>
#include <map>
#include <algorithm>
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
	map[".html"] = "text/html";
	map[".htm"] = "text/html";
	map[".css"] = "text/css";
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

static std::string httpCodeToStr(int httpcode) {
	switch (httpcode) {
        // 1xx Informational
        case 100: return "Continue";
        case 101: return "Switching Protocols";
        case 102: return "Processing";
        case 103: return "Early Hints";

        // 2xx Success
        case 200: return "OK";
        case 201: return "Created";
        case 202: return "Accepted";
        case 203: return "Non-Authoritative Information";
        case 204: return "No Content";
        case 205: return "Reset Content";
        case 206: return "Partial Content";
        case 207: return "Multi-Status";
        case 208: return "Already Reported";
        case 226: return "IM Used";

        // 3xx Redirection
        case 300: return "Multiple Choices";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 303: return "See Other";
        case 304: return "Not Modified";
        case 305: return "Use Proxy";
        case 307: return "Temporary Redirect";
        case 308: return "Permanent Redirect";

        // 4xx Client Errors
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 402: return "Payment Required";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 406: return "Not Acceptable";
        case 407: return "Proxy Authentication Required";
        case 408: return "Request Timeout";
        case 409: return "Conflict";
        case 410: return "Gone";
        case 411: return "Length Required";
        case 412: return "Precondition Failed";
        case 413: return "Payload Too Large";
        case 414: return "URI Too Long";
        case 415: return "Unsupported Media Type";
        case 416: return "Range Not Satisfiable";
        case 417: return "Expectation Failed";
        case 418: return "I'm a Teapot";
        case 421: return "Misdirected Request";
        case 422: return "Unprocessable Entity";
        case 423: return "Locked";
        case 424: return "Failed Dependency";
        case 425: return "Too Early";
        case 426: return "Upgrade Required";
        case 428: return "Precondition Required";
        case 429: return "Too Many Requests";
        case 431: return "Request Header Fields Too Large";
        case 451: return "Unavailable For Legal Reasons";

        // 5xx Server Errors
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        case 505: return "HTTP Version Not Supported";
        case 506: return "Variant Also Negotiates";
        case 507: return "Insufficient Storage";
        case 508: return "Loop Detected";
        case 510: return "Not Extended";
        case 511: return "Network Authentication Required";

        // Default case for unknown codes
        default: return "";
    }
} 

static std::string getResPage(int httpCode) {
	std::string error;

	error += "<!DOCTYPE html>";
	error += "<html>";
	error += "<head>";
	error += "<title>" + httpCodeToStr(httpCode) + "</title>";
	error += "</head>";
	error += "<body>";
	error += "<p>" + StringUtils::itoa(httpCode) + " " + httpCodeToStr(httpCode) + "</p>";
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
	if (httpCode >= 300) {
		const std::string &page = getResPage(httpCode);
		str += "content-type: text/html\r\n";
		addContentLength(str, page.size());
		str += page;
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
	sort(arr.begin(), arr.end());
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
