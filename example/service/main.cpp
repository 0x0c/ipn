#include <ipn.hpp>

using namespace m2d;

int main(int argc, char *argv[])
{
	try {
		if (argc != 2) {
			std::cerr << "Usage: service <file>\n";
			return 1;
		}

		auto echo_service = ipn::service<ipn::simple_request_t, ipn::simple_response_t>(argv[1]);
		echo_service.run([](ipn::simple_request_t &req) {
			std::cout << "request: " << req.message << std::endl;
			return ipn::simple_response_t(req.message);
		});
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
