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
		auto pub = ipn::publisher<message>(argv[1]);
		while (true) {
			auto msg = message("hello");
			pub.send("topic_1", msg);
			std::cout << "send " << msg.text << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
