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

		auto echo_service = ipn::service<example::request, example::response>(argv[1]);
		echo_service.run([](example::request &req) {
			std::cout << "request: " << req.message() << std::endl;
			example::response response;
			response.set_message(req.message());
			return response;
		});
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
