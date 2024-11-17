#include <cstdio>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sstream>
#include <ctime>
#include <ios>
#include <unistd.h>
#include <signal.h>
#include <iomanip>
#include <vector>
#include <sys/stat.h>
#include "CGI.hpp"
#include "Log.hpp"
#include "StringUtils.hpp"

CGI::~CGI() {
	if (_cgiFd[0] == -1) return;
	close(_cgiFd[0]);
	close(_cgiFd[1]);
	killCGI();
}

static bool isFile(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

CGI::CGI(const std::string &str, LocationConfig &locationConfig, Socket &client,
	const std::map<std::string, std::string> &urlParams):
	_locationConfig(locationConfig), _client(client), _urlParams(urlParams) {
	_cgiFd[0] = -1;
	_cgiFd[1] = -1;
	_ready = false;
	_scriptPath = str;
	_pid = -1;
	_exitCode = -1;
	setCGI();
	if (!isFile(_scriptPath)) {
		_scriptPath = "";
		_binPath = "";
	}
}

std::string CGI::createQueryString() {
	std::string str;

	for (std::map<std::string, std::string>::const_iterator it = _urlParams.begin();
		it != _urlParams.end(); it++) {
		std::string key = it->first;
		std::string value = it->second;
		str += it->first + "=" + it->second + "&";
	}
	return str;
}

void	CGI::child() {
	srand(time(0));
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(8) << rand();
	std::string filename = "/tmp/webserv" + ss.str();
	int fd = open(filename.c_str(), O_CREAT | O_TRUNC | O_RDWR);
	std::string &cgiBody = _client.request.cgiBody;

	write(fd, cgiBody.c_str(), cgiBody.size());
	lseek(fd, 0, SEEK_SET);
	close(_cgiFd[0]);
	dup2(_cgiFd[1], 1);
	close(_cgiFd[1]);
	dup2(fd, 0);
	close(fd);

	const char *argv[3];
	argv[0] = _binPath.c_str();
	argv[1] = _scriptPath.c_str();
	argv[2] = NULL;
	std::vector<std::string> envVec;
    envVec.push_back("PATH_INFO=" + _args);
    envVec.push_back("REQUEST_METHOD=" + _client.request.method);
    envVec.push_back("CONTENT_LENGTH=" + StringUtils::itoa(cgiBody.size()));
    envVec.push_back("SCRIPT_NAME=" + _scriptPath);
    envVec.push_back("SCRIPT_FILENAME=" + _scriptPath);
    envVec.push_back("REDIRECT_STATUS=200");
    envVec.push_back("CONTENT_TYPE=" + _client.request.headerArguments["content-type"]);
    envVec.push_back("QUERY_STRING=" + createQueryString());

	const char *env[envVec.size() + 1];
	for (size_t i = 0; i < envVec.size(); i++)
		env[i] = envVec[i].c_str();
    env[envVec.size()] = NULL;
	execve(_binPath.c_str(), const_cast<char**>(argv), const_cast<char**>(env));
	throw std::runtime_error("CGI failed: " + _binPath);
	return;
}

void	CGI::exec() {
	Log::Trace("CGI called: " + _binPath + " " + _scriptPath);
	createPipe();
	_pid = fork();

	if (_pid == 0) child();
}

void	CGI::setArgs(const std::vector<std::string> &arr, size_t &i) {
	i++;
	for (;i < arr.size(); i++) {
		_args += arr[i];
	}
}

void CGI::setCGI() {
	std::string str;
	std::vector<std::string> arr = StringUtils::split(_scriptPath, "/", true);

	if (arr.size())
		arr[0].erase(0, 1);
	for (size_t i = 0; i < arr.size(); i++) {
		str += arr[i];
		size_t pos = str.find_last_of(".");
		if (pos == std::string::npos) continue;

		std::string substr = str.substr(pos);
		std::map<std::string, std::string>::iterator it = _locationConfig.cgi.find(substr);
		if (it != _locationConfig.cgi.end()) {
			_scriptPath = str;
			_binPath = it->second;
			setArgs(arr, i);
			return;
		}
	}
	_scriptPath = "";
}

bool	CGI::isReady() {
	if (_pid == -1) return false;
	int status = -1;

	if (waitpid(_pid, &status, WNOHANG) == _pid) {
		_ready = true;
		if (WIFEXITED(status))
			_exitCode = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			_exitCode = -WTERMSIG(status);
	}
	return _ready;
}

void	CGI::killCGI() {
	if (_pid != -1 && _pid != 0)
		kill(_pid, SIGTERM);
}

const std::string &CGI::getScriptPath() const {
	return _scriptPath;
}

const std::string	&CGI::getArgs() const {
	return _args;
}

const std::string	&CGI::getBinPath() const {
	return _binPath;
}

const int	(&CGI::getCgiFd() const)[2] {
	return _cgiFd;
}

void	CGI::createPipe() {
	pipe2(_cgiFd, O_NONBLOCK);
}
