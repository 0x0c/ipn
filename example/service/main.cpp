#include <ipn.hpp>

#include "../req_rep.hpp"

using namespace m2d;

int main(int argc, char *argv[])
{
	try {
		if (argc != 2) {
			std::cerr << "Usage: service <file>\n";
			return 1;
		}

		auto echo_service = ipn::service<simple_request_t, simple_response_t>(argv[1]);
		echo_service.run([](simple_request_t &req) {
			std::cout << "request: " << req.message << std::endl;
			return simple_response_t(req.message);
		});
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
