#include <iostream>

#include "../include/client.hpp"

using namespace m2d;

ipn::client::client(const std::string &file)
    : socket_(io_context_)
    , endpoint_(file)
{
}

ipn::client::~client()
{
}

void ipn::client::wait_for_server()
{
	boost::system::error_code ec;

	// 接続しに行く
	do {
		socket_.connect(endpoint_, ec);
		if (ec) {
			std::cout << "could not connect" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	} while (ec);
}