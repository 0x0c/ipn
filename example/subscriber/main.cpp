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

		while (true) {
			auto sub = std::make_shared<ipn::subscriber<example::message>>(argv[1]);
			auto idx = sub->subscribe(ipn::topic::all, [](example::message msg) {
				std::cout << "received " << msg.text() << std::endl;
			});
			std::cout << "start subscribe: " << idx << std::endl;

			sleep(5);
			break;
			sub->dispose(idx);
			if (sub->is_disposed(idx)) {
				std::cout << "disposed" << std::endl;
				break;
			}
		}
		while (true) {
		}
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
