#pragma once

#include <boost/asio.hpp>
#include <functional>
#include <msgpack.hpp>

namespace m2d
{
namespace ipn
{
	using boost::asio::local::stream_protocol;
	class session : public std::enable_shared_from_this<session>
	{
	public:
		session(stream_protocol::socket sock);
		~session();

		void start(std::function<void(msgpack::object, std::shared_ptr<session> ses)> handler);
		void send(msgpack::object data);
		void close();

	private:
		std::size_t const window_size = 1024;
		stream_protocol::socket socket_;
		std::function<void(msgpack::object, std::shared_ptr<session> ses)> handler_;
		msgpack::unpacker unp_;
		std::vector<std::string> subscribing_topics_;

		void wait();
	};

	class server
	{
	public:
		server(const std::string &file);
		void run();

	private:
		void accept();

		boost::asio::io_context io_context_;
		stream_protocol::acceptor acceptor_;
		std::vector<std::shared_ptr<ipn::session>> sessions_;
		std::map<std::string, std::vector<std::shared_ptr<ipn::session>>> subscribers_;
	};
} // namespace ipn
} // namespace m2d
