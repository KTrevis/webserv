#include "Log.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

void	onSigint(int signal) {
	static_cast<void>(signal);
	throw std::runtime_error("");
}

int	main(int ac, char **av) {
	if (ac != 2) return dprintf(2, "%s [port]\n", av[0]);
	signal(SIGINT, onSigint);
	try {
		ConfigParser config(av[1]);
		Server server(config.getConfigs());
		server.start();
	} catch (std::exception &e) {
		if (strcmp(e.what(), ""))
			Log::Error(e.what());
	}
}
