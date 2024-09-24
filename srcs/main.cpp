#include "Server.hpp"
#include <cstdlib>
#include <iostream>

int	main() {
	try {
		Server server(16384);
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
