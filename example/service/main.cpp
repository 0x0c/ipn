#include <chrono>

#include <ipn.hpp>

#include "ReqRep.pb.h"

using namespace m2d;

int main(int argc, char *argv[])
{
	try {
		if (argc != 2) {
			std::cerr << "Usage: service <file>\n";
			return 1;
		}

		auto echo_service = ipn::service<example::request, example::response>::create(argv[1]);
		echo_service->run([](boost::optional<example::request> req) {
			if (req) {
				std::cout << "request: " << (*req).message() << std::endl;
				example::response response;
				response.set_message("echo " + (*req).message());
				return response;
			}

			std::cout << "Invalid request" << std::endl;
			example::response response;
			response.set_message("error: invalid request");
			return response;
		});

		int counter = 0;
		while (echo_service->is_disposed() == false) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			counter++;
			if (counter == 5) {
				echo_service->dispose();
			}
		}
		std::cout << "disposed" << std::endl;
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
