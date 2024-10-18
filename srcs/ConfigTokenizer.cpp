#include "ConfigParser.hpp"
#include <exception>
#include <fstream>
#include <stdexcept>

bool	ConfigParser::isToken(const char &c) {
	return c == '{' || c == '}' || c == ';';
}

static void	skipWhiteSpace(int &i, const std::string &str) {
	while (str[i] && std::iswspace(str[i]))
		i++;
}

static std::string	getWord(int &i, const std::string &str) {
	std::string	word;

	if ( ConfigParser::isToken(str[i])) {
		if (str[i] != ';')
			word += str[i];
		i++;
		return word;
	}
	while (str[i] && !std::iswspace(str[i]) && !ConfigParser::isToken(str[i])) {
		word += str[i];
		i++;
	}
	return word;
}

static std::vector<std::string>	tokenizeLine(const std::string &str) {
	std::vector<std::string>	list;

	for (int i = 0; str[i];) {
		skipWhiteSpace(i, str);
		const std::string &word = getWord(i, str);
		if (word != "") list.push_back(word);
	}
	return list;
}

static std::vector<std::string>	getFile(const std::string &filename) {
	std::ifstream			stream(filename.c_str());
	std::string				buffer;
	std::vector<std::string>	file;

	while (std::getline(stream, buffer))
		file.push_back(buffer);
	return file;
}

std::string	ConfigParser::tokenizeFile(const std::string &filename) {
	const std::vector<std::string> &file = getFile(filename);
	if (file.size() == 0)
		throw std::runtime_error("Failed to read file.");
	std::vector<std::string>::const_iterator it = file.begin();

	while (it != file.end()) {
		std::vector<std::string> list = tokenizeLine(*it);
		if (list.size() != 0)
			_lines.push_back(list);
		it++;
	}
	return "";
}
 
