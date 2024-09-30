#pragma once

#include <iostream>

enum e_loglevel {
	ERROR,
	INFO,
	DEBUG,
	TRACE,
};

#ifndef LOG_LEVEL
# define LOG_LEVEL TRACE
#endif

namespace Log {
	void		Error(const std::string &str);
	void		Info(const std::string &str);
	void		Debug(const std::string &str);
	void		Trace(const std::string &str);
	void		Event(int maskEvent);
};
