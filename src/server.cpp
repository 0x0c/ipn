#include <iostream>

#include "../include/ipn.hpp"

using namespace m2d;

ipn::session::session(stream_protocol::socket sock)
    : socket_(std::move(sock))
{
}

ipn::session::~session()
{
	std::cout << "deinit" << std::endl;
}

void ipn::session::wait()
{
	// clientからデータが送られてくるのを待つ
	unp_.reserve_buffer(window_size);

	auto self(shared_from_this());
	boost::system::error_code ec;
	size_t length = boost::asio::read(socket_, boost::asio::buffer(unp_.buffer(), window_size), boost::asio::transfer_at_least(1), ec);
	if (!ec) {
		unp_.buffer_consumed(length);
		msgpack::object_handle oh;
		std::cout << "wait success" << std::endl;
		while (unp_.next(oh)) {
			handler_(oh.get(), self);
		}
	}
	else {
		std::cout << "wait error: " << ec << std::endl;
	}
}

void ipn::session::start(std::function<void(msgpack::object, std::shared_ptr<session> ses)> handler)
{
	handler_ = std::move(handler);
	wait();
}

void ipn::session::send(msgpack::object data)
{
	msgpack::sbuffer buffer;
	msgpack::pack(buffer, data);
	boost::system::error_code ec;
	auto self(shared_from_this());
	boost::asio::write(socket_, boost::asio::buffer(buffer.data(), buffer.size()), ec);
	if (!ec) {
		std::cout << "send success" << std::endl;
	}
	else {
		std::cout << "send error: " << ec << std::endl;
	}
}

void ipn::session::close()
{
	socket_.close();
}

ipn::server::server(const std::string &file)
    : acceptor_(io_context_, stream_protocol::endpoint(file))
{
	accept();
}

void ipn::server::run()
{
	io_context_.run();
}

void ipn::server::accept()
{
	std::cout << "wait" << std::endl;
	auto handler = [this](boost::system::error_code ec, stream_protocol::socket socket) {
		if (!ec) {
			std::cout << "start session: " << std::endl;
			auto session = std::make_shared<ipn::session>(std::move(socket));
			sessions_.push_back(session);
			session->start([this](msgpack::object obj, std::shared_ptr<ipn::session> ses) {
				std::cout << obj << std::endl;
				msgpack::object_map obj_map = obj.via.map;

				if (obj_map.size < 2) {
					std::cout << "invalid message" << std::endl;
					return;
				}

				std::string action;
				(obj_map.ptr[0].val).convert(action);

				std::string topic;
				(obj_map.ptr[1].val).convert(topic);

				if (action == "subscribe") {
					if (this->subscribers_.find(topic) != this->subscribers_.end()) {
						this->subscribers_.at(topic).push_back(ses);
					}
					else {
						std::vector<std::shared_ptr<ipn::session>> subscribers;
						subscribers.push_back(ses);
						this->subscribers_.insert(std::make_pair(topic, std::move(subscribers)));
					}
					std::cout << "subscribe" << std::endl;
				}
				else if (action == "broadcast") {
					std::cout << "broadcast: " << topic << std::endl;
					auto data = obj_map.ptr[2].val;
					if (this->subscribers_.find(topic) != this->subscribers_.end()) {
						std::cout << "broadcast found" << std::endl;
						for (auto ses : this->subscribers_.at(topic)) {
							ses->send(data);
						}
					}
				}
				else if (action == "close") {
					// close socket and remove from sessions_ and subscribers_
					ses->close();
					//                        sessions_.erase(<#const_iterator __position#>)
				}
			});
		}

		accept();
	};

	acceptor_.async_accept(handler);
}