
#include <chrono>
#include <iostream>

#include <ipn.hpp>

#include "../message.hpp"

using namespace m2d;

int main(int argc, char *argv[])
{
	try {
		if (argc != 2) {
			std::cerr << "Usage: stream_server <file>\n";
			std::cerr << "*** WARNING: existing file is removed ***\n";
			return 1;
		}

		std::cout << "start monitoring" << std::endl;

		auto sub = ipn::subscriber<message>(argv[1]);
//		sub.subscribe(ipn::topic::all, [](message msg) {
//			std::cout << "received " << msg.text << std::endl;
//		});
		while (true) {
            std::cout << "check: ";
			if (sub.is_publisher_available()) {
				std::cout << "available" << std::endl;
			}
			else {
				std::cout << "not available" << std::endl;
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
