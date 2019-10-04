#pragma once

#include <boost/asio.hpp>
#include <functional>
#include <msgpack.hpp>

namespace m2d
{
namespace ipn
{
	using boost::asio::local::stream_protocol;
	class client : public std::enable_shared_from_this<client>
	{
	public:
		client(const std::string &file);
		~client();

		void wait_for_server();

	private:
		std::size_t const window_size = 1024;
		stream_protocol::socket socket_;
		boost::asio::io_context io_context_;
		stream_protocol::endpoint endpoint_;
	};

} // namespace ipn
} // namespace m2d
