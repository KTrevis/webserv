/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ketrevis <ketrevis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 13:28:32 by ketrevis          #+#    #+#             */
/*   Updated: 2024/11/11 14:14:57 by ketrevis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdio>
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
#include "CGI.hpp"
#include "Log.hpp"
#include "StringUtils.hpp"

CGI::~CGI() {
	if (_cgiFd[0] == -1) return;
	close(_cgiFd[0]);
	close(_cgiFd[1]);
}

CGI::CGI(const std::string &str, LocationConfig &locationConfig, Socket &client,
	const std::map<std::string, std::string> &urlParams):
	_locationConfig(locationConfig), _client(client), _urlParams(urlParams) {
	_cgiFd[0] = -1;
	_cgiFd[1] = -1;
	_ready = false;
	_scriptPath = str;
	_pid = -1;
	setCGI();
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

void	CGI::child(Socket &client) {
	(void)client;
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

	const char *argv[2];
	argv[0] = _binPath.c_str();
	argv[1] = NULL;
	std::vector<std::string> envVec;
    envVec.push_back("PATH_INFO=" + _args);
    envVec.push_back("REQUEST_METHOD=" + client.request.method);
    envVec.push_back("CONTENT_LENGTH=" + StringUtils::itoa(cgiBody.size()));
    envVec.push_back("SCRIPT_NAME=" + _scriptPath);
    envVec.push_back("SCRIPT_FILENAME=" + _scriptPath);
    envVec.push_back("REDIRECT_STATUS=200");
    envVec.push_back("CONTENT_TYPE=" + client.request.headerArguments["content-type"]);

    // Allocate and assign to the env array
    const char *env[envVec.size() + 1];
	for (size_t i = 0; i < envVec.size(); i++)
		env[i] = envVec[i].c_str();
    env[envVec.size()] = NULL;  // Null-terminate the array

	execve(_binPath.c_str(), const_cast<char**>(argv), const_cast<char**>(env));
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
