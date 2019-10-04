#include <iostream>

#include "include/ipn.hpp"

using namespace m2d;

using boost::asio::local::stream_protocol;
constexpr std::size_t max_length = 1024;
int main(int argc, char *argv[])
{
	if (argc != 2) {
		std::cerr << "Usage: stream_client <file>\n";
		return 1;
	}

	boost::asio::io_context io_context;
	boost::system::error_code ec;

	// 接続しに行く
	stream_protocol::socket s(io_context);

	do {
		s.connect(stream_protocol::endpoint(argv[1]), ec);
		if (ec) {
			std::cout << "could not connect" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	} while (ec);
	std::cout << "accepted" << std::endl;

	msgpack::sbuffer buffer;
	msgpack::packer<msgpack::sbuffer> pk2(&buffer);
	pk2.pack_map(2);
	pk2.pack(std::string("action"));
	pk2.pack(std::string("subscribe"));
	pk2.pack(std::string("topic"));
	pk2.pack(std::string("mute"));

	s.write_some(boost::asio::buffer(buffer.data(), buffer.size()), ec);
	if (!ec) {
		std::cout << "sent" << std::endl;
	}
	else {
		std::cout << "error: " << ec << std::endl;
	}

	while (1) {
		msgpack::unpacker unp;
		unp.reserve_buffer(max_length);
		size_t reply_length = boost::asio::read(s, boost::asio::buffer(unp.buffer(), max_length), boost::asio::transfer_at_least(1), ec);
		unp.buffer_consumed(reply_length);
		std::cout << "Reply is: ";
		msgpack::object_handle oh;
		while (unp.next(oh)) {
			std::cout << oh.get();
		}
		std::cout << "\n";
	}
	std::cout << "done" << std::endl;

	return 0;
}
