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

		auto service = ipn::service<ipn::request, message>(argv[1]);
		service.run([](ipn::request &req) {
			std::cout << "request: " << req.message << std::endl;
			return message(req.message);
		});
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
