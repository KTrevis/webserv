#pragma once

#include <iostream>

enum e_methods {
	GET = 1,
	POST = 2,
	DELETE = 4,
};

class Request
{
	public:
		Request();
		~Request();
	private:
		/* e_methods	_method; */
		/* std::string _path; */
		/* std::string _httpVer; */
		/* std::string _plainTxt; */
};
