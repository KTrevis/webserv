#include "CGI.hpp"
#include <vector>
#include "StringUtils.hpp"

CGI::CGI(const std::string &str, LocationConfig &locationConfig): _locationConfig(locationConfig) {
	_cgiPath = str;
	setCGI();
	std::cout << _cgiPath << std::endl;
	std::cout << _args << std::endl;;
}

void	CGI::setArgs(const std::vector<std::string> &arr, size_t &i) {
	i++;
	for (;i < arr.size(); i++) {
		_args += arr[i];
	}
}

void CGI::setCGI() {
	std::string str;
	std::vector<std::string> arr = StringUtils::split(_cgiPath, "/", true);
	arr[0].erase(0, 1);

	for (size_t i = 0; i < arr.size(); i++) {
		str += arr[i];
		size_t pos = str.find_last_of(".");
		if (pos == std::string::npos) continue;
		std::string substr = str.substr(pos);
		if (_locationConfig.cgi.find(substr) != _locationConfig.cgi.end()) {
			_cgiPath = str;
			setArgs(arr, i);
			return;
		}
	}
	_cgiPath = "";
}

const std::string &CGI::getCgiPath() {
	return _cgiPath;
}
const std::string	&CGI::getArgs() {
	return _args;
}
