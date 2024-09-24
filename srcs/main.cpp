#include "Server.hpp"
#include <cstdlib>
#include <iostream>

int	main(int ac, char **av) {
	if (ac != 2) {
		std::cerr << "usage: " << av[0] << " [port]" << std::endl;
		return 1;
	}
	Server server(atoi(av[1]));
}
