#include "NetworkUtils.hpp"
#include "Server.hpp"
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
		Server server(atoi(av[1]));
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
