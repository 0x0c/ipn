#include <chrono>
#include <iostream>

#include <ipn.hpp>

#include "ReqRep.pb.h"

using namespace m2d;

int main(int argc, char *argv[])
{
	try {
		if (argc != 2) {
			std::cerr << "Usage: publisher <file>\n";
			return 1;
		}
		int counter = 0;
		auto pub = ipn::publisher<example::message>(argv[1]);
		while (true) {
			example::message msg;
			msg.set_text("hello " + std::to_string(counter++ % 10));
			pub.send("topic_1", msg);
			std::cout << "send " << msg.text() << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
