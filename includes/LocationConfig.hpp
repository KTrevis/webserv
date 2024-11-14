#pragma once

#include <iostream>
#include <map>
#include <vector>

typedef std::vector<std::string> StringVector;


class LocationConfig {
	public:
	LocationConfig();
	LocationConfig(size_t &i, const std::vector<StringVector> &lines, const std::string &name);
	int									methodMask;
	bool								autoIndex;
	std::string							root;
	std::string							indexFile;
	std::string							uploadPath;
	std::string							alias;
	// key is the extension (.py/.php...) and value is the cgi path
	std::map<std::string, std::string>	cgi;
	std::string							name;
	std::string							redirection;
	int									maxBodySize;
	std::map<int, std::string>			errorPages;
	void								displayData();
	private:
	bool	setErrorPage(const StringVector &arr);
	bool	setMethods(const StringVector &arr);
	bool	setRoot(const StringVector &arr);
	bool	setIndex(const StringVector &arr);
	bool	setCGI(const StringVector &arr);
	bool	setUploadPath(const StringVector &arr);
	bool	setRedirection(const StringVector &arr);
	bool	setAlias(const StringVector &arr);
	bool	setAutoIndex(const StringVector &arr);
};
