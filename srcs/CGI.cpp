#include "CGI.hpp"
#include <cstdio>
#include <sys/wait.h>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <ctime>
#include <ios>
#include <unistd.h>
#include <signal.h>
#include <iomanip>
#include <vector>
#include "Log.hpp"
#include "StringUtils.hpp"
#include "Server.hpp"

CGI::~CGI() {
	if (_cgiFd[0] == -1) return;
	close(_cgiFd[0]);
	close(_cgiFd[1]);
}

CGI::CGI(const std::string &str, LocationConfig &locationConfig, Socket &client):
	_locationConfig(locationConfig), _client(client) {
	_cgiFd[0] = -1;
	_cgiFd[1] = -1;
	_ready = false;
	_scriptPath = str;
	_pid = -1;
	setCGI();
}

void	CGI::child(Socket &client) {
	(void)client;
	srand(time(0));
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(8) << rand();
	std::string filename = "/tmp/webserv" + ss.str();
	int fd = open(filename.c_str(), O_CREAT | O_TRUNC | O_RDWR);
	std::string &cgiBody = _client.request.cgiBody;

	write(fd, cgiBody.c_str(), cgiBody.size());
	close(_cgiFd[0]);
	dup2(_cgiFd[1], 1);
	close(_cgiFd[1]);
	dup2(fd, 0);
	close(fd);

	char **argv = new char*[3];
	argv[0] = strdup(_binPath.c_str());
	argv[2] = NULL;

	char **env = new (char *[2]);
	env[0] = strdup(std::string("PATH_INFO=" + _args).c_str());
	env[1] = strdup(std::string("REQUEST_METHOD=" + client.request.method).c_str());
	env[2] = strdup(std::string("CONTENT_LENGTH=" + StringUtils::itoa(cgiBody.size())).c_str());
	env[3] = strdup(std::string("SCRIPT_NAME=" + _scriptPath).c_str());
	env[4] = strdup(std::string("SCRIPT_FILENAME=" + _scriptPath).c_str());
	env[5] = strdup("REDIRECT_STATUS=200");
	env[6] = NULL;

	execve(_binPath.c_str(), argv, env);
	free(argv[0]);
	free(argv[1]);
	free(env[0]);
	delete[] env;
	delete[] argv;
	return;
}

void	CGI::exec() {
	Log::Trace("CGI called: " + _binPath + " " + _scriptPath);
	createPipe();
	_pid = fork();

	if (_pid == 0) child(_client);
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
	if (waitpid(_pid, NULL, WNOHANG) == _pid)
		_ready = true;
	return _ready;
}

void	CGI::killCGI() {
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
	pipe(_cgiFd);
	int flags = fcntl(_cgiFd[0], F_GETFL);
	fcntl(_cgiFd[0], F_SETFL, flags | O_NONBLOCK);
	flags = fcntl(_cgiFd[1], F_GETFL);
	fcntl(_cgiFd[1], F_SETFL, flags | O_NONBLOCK);
}
