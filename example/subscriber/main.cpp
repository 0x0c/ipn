#include <iostream>
#include <ipn.hpp>

#include "ReqRep.pb.h"

using namespace m2d;

int main(int argc, char *argv[])
{
	try {
		if (argc != 2) {
			std::cerr << "Usage: subscriber <file>\n";
			return 1;
		}

		auto sub = ipn::subscriber<example::message>(argv[1]);
		sub.subscribe(ipn::topic::all, [](example::message msg) {
			std::cout << "received " << msg.text() << std::endl;
		});
		while (true) {
		}
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
