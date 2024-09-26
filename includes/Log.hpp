#pragma once

#include <iostream>

enum e_loglevel {
	ERROR,
	INFO,
	DEBUG,
};

#ifndef LOG_LEVEL
# define LOG_LEVEL DEBUG
#endif

namespace Log {
	void		Error(const std::string &str);
	void		Info(const std::string &str);
	void		Debug(const std::string &str);
	void		Event(int maskEvent);
};
