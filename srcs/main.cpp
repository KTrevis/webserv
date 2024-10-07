#include "Log.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"
#include <cstdlib>
#include <signal.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

void	onSigint(int signal) {
	(void)signal;
	errno = SIGINT;
}

int	main(int ac, char **av) {
	if (ac != 2) return dprintf(2, "%s [port]\n", av[0]);
	/* signal(SIGINT, onSigint); */
	try {
		ConfigParser config(av[1]);
		/* Server server(atoi(av[1])); */
		/* server.start(); */
	} catch (std::exception &e) {
		Log::Error(e.what());
	}
}
