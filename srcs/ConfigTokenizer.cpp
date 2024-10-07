#include "ConfigParser.hpp"

static bool	isToken(const char &c) {
	return c == '{' || c == '}' || c == ';';
}

static void	skipWhiteSpace(int &i, const std::string &str) {
	while (str[i] && std::iswspace(str[i]))
		i++;
}

static std::string	getWord(int &i, const std::string &str) {
	std::string	word;

	if (isToken(str[i])) {
		word += str[i];
		i++;
		return word;
	}
	while (str[i] && !std::iswspace(str[i]) && !isToken(str[i])) {
		word += str[i];
		i++;
	}
	return word;
}

static std::list<std::string>	tokenizeLine(const std::string &str) {
	std::list<std::string>	list;

	for (int i = 0; str[i];) {
		skipWhiteSpace(i, str);
		list.push_back(getWord(i, str));
	}
	return list;
}

std::string	ConfigParser::tokenizeFile(std::list<std::string> &file) {
	std::list<std::string>::iterator it = file.begin();

	while (it != file.end()) {
		std::list<std::string> list = tokenizeLine(*it);
		if (list.size() != 0)
			_lines.push_back(list);
		it++;
	}
	return "";
}

