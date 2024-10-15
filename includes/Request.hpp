#pragma once

#include <iostream>

class Request
{
	private:
		std::string _method;
		std::string _path;
		std::string _httpVer;
		std::string _plainTxt;

	public:
		Request();
		~Request();
};
