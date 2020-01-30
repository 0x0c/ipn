#include <chrono>
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

		auto sub = ipn::subscriber<example::message>::create(argv[1]);
		auto idx = sub->subscribe(ipn::topic::all, [](example::message msg) {
			std::cout << "received " << msg.text() << std::endl;
		});
		std::cout << "start subscribe: " << idx << std::endl;
		int counter = 0;
		while (sub->is_disposed(idx) == false) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			counter++;
			if (counter == 5) {
				sub->dispose(idx);
			}
		}
		std::cout << "disposed" << std::endl;
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
