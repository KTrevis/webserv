#pragma once

#include <iostream>
#include <map>

struct Redirection {
	int			codeHTTP;
	std::string	toRedirect;
};

class LocationConfig {
	std::string							location;
	std::string							root;
	std::map<std::string, std::string>	cgiPaths; // key is the extension (.py/.php...) and value is the cgi path
	std::string							indexFile;
	std::string							uploadPath;
	Redirection							redirection;
};
