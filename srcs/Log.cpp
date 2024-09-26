#include "Log.hpp"

void	Log::Info(const std::string &str) {
	if (LOG_LEVEL >= INFO)
		std::cout << "[INFO]: " << str << std::endl;
}

void	Log::Debug(const std::string &str) {
	if (LOG_LEVEL >= DEBUG)
		std::cout << "[DEBUG]: " << str << std::endl;
}

void	Log::Error(const std::string &str) {
	if (LOG_LEVEL >= ERROR)
		std::cerr << "[ERROR]: " << str << std::endl;
}

