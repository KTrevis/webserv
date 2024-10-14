#pragma once

#include <iostream>

class Request
{
private:
	std::string _method;
	std::string _path;
	std::string _httpVer;
// continue the request and continue to parse at home noob
public:
	Request();
	~Request();
};
