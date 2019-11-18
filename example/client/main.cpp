#include <chrono>

#include <ipn.hpp>

#include "../req_rep.hpp"

using namespace m2d;

int main(int argc, char *argv[])
{
	try {
		if (argc != 2) {
			std::cerr << "Usage: client <file>\n";
			return 1;
		}

		auto client = ipn::client<simple_request_t, simple_response_t>(argv[1]);
		int sequence = 5;
		while (sequence--) {
			auto req = simple_request_t("hello");
			ipn::result_t<simple_response_t> result = client.send(req);

			if (result.error) {
				std::cout << "E: " << result.error->description << std::endl;
			}
			else {
				simple_response_t &msg = *(result.response);
				std::cout << "response: " << msg.message << std::endl;
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
