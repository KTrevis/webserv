#pragma once

#include <vector>
#include <iostream>

class ServerConfig;
class Server;
class Socket;

namespace Utils {
	template<typename T>
	void displayVector(const std::vector<T> &arr) {
		for (size_t i = 0; i < arr.size(); i++)
			std::cout << arr[i] << std::endl;
	}
	ServerConfig &getServerConfig(Server &server, Socket &client);
	bool	isFolder(const std::string &name);
	bool	isFile(const std::string& path);
};
