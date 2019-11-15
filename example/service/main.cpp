#include <ipn.hpp>

#include "../message.hpp"

using namespace m2d;

int main(int argc, char *argv[])
{
	try {
		if (argc != 2) {
			std::cerr << "Usage: service <file>\n";
			return 1;
		}

		auto echo_service = ipn::service<ipn::simple_request, ipn::simple_response>(argv[1]);
		echo_service.run([](ipn::simple_request &req) {
			std::cout << "request: " << req.message << std::endl;
			return ipn::simple_response(req.message);
		});
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
