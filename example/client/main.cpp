#include <chrono>

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

		auto client = ipn::client<ipn::request, message>(argv[1]);
		int sequence = 5;
		while (sequence--) {
			auto req = ipn::request("hello");
			client.send(req, [](message msg, bool success) {
				if (success) {
					std::cout << "response: " << msg.text << std::endl;
				}
				else {
					std::cout << "E: malformed reply from server: " << std::endl;
				}
			});
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
