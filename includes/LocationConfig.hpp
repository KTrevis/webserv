#pragma once

#include <iostream>
#include <map>
#include <vector>

typedef std::vector<std::string> StringVector;

enum e_methods {
	GET = 1,
	POST = 2,
	DELETE = 4,
};

struct Redirection {
	int			codeHTTP;
	std::string	toRedirect;
};

class LocationConfig {
	public:
		LocationConfig(size_t &i, const std::vector<StringVector> &lines);
		int									methodMask;
		std::string							root;
		std::string							indexFile;
		std::string							uploadPath;
		// key is the extension (.py/.php...) and value is the cgi path
		std::map<std::string, std::string>	cgi; 
		Redirection							redirection;
	private:
		bool	setMethods(const StringVector &str);
		bool	setRoot(const StringVector &arr);
		bool	setIndex(const StringVector &arr);
		bool	setCGI(const StringVector &arr);
		bool	setUploadPath(const StringVector &arr);
		bool	setRedirection(const StringVector &arr);
};
