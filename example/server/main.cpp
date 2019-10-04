#include <iostream>

#include "include/ipn.hpp"

using namespace m2d;

int main(int argc, char *argv[])
{
	std::cout << "start server" << std::endl;
	try {
		if (argc != 2) {
			std::cerr << "Usage: stream_server <file>\n";
			std::cerr << "*** WARNING: existing file is removed ***\n";
			return 1;
		}

		std::remove(argv[1]);
		ipn::server s(argv[1]);
		s.run();
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
