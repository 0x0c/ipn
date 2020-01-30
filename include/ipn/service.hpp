#pragma once

#include <functional>
#include <iostream>
#include <thread>

#include <google/protobuf/message.h>
#include <msgpack.hpp>
#include <zmq.hpp>

#include "constant.hpp"

namespace m2d
{
namespace ipn
{
	template <typename Request, typename Response>
	class service : public std::enable_shared_from_this<service<Request, Response>>
	{
	private:
		std::string endpoint_;
		bool disposed_ = false;
		bool running_ = false;

	public:
		std::function<void(zmq::error_t &e)> error_handler;

		service(const std::string &endpoint)
		    : endpoint_(ipn::rep_endpoint(endpoint))
		{
			static_assert(std::is_base_of<google::protobuf::Message, Request>::value, "Request not derived from google::protobuf::Message");
			static_assert(std::is_base_of<google::protobuf::Message, Response>::value, "Response not derived from google::protobuf::Message");
		}

		bool run(std::function<Response(Request &)> handler)
		{
			if (running_) {
				return false;
			}
			std::thread t([=] {
				std::weak_ptr<service<Request, Response>> weak_this = this->shared_from_this();
				try {
					auto shared_this = weak_this.lock();
					if (!shared_this) {
						return;
					}

					zmq::socket_t rep(*shared_ctx(), zmq::socket_type::rep);
					rep.bind(shared_this->endpoint_);

					shared_this->running_ = true;
					while (shared_this->is_disposed() == false) {
						zmq::message_t msg;
						auto result = rep.recv(msg);
						if (result) {
							Request req;
							auto data_msg = static_cast<const char *>(msg.data());
							std::string data_str(data_msg, msg.size());
							req.ParseFromString(data_str);

							auto res = handler(req);

							// pack to zmq::message_t
							auto serialized_string = res.SerializeAsString();
							auto size = serialized_string.size() * sizeof(std::string::value_type);
							zmq::message_t reply_msg(size);
							std::memcpy(reply_msg.data(), serialized_string.c_str(), size);
							rep.send(reply_msg, zmq::send_flags::none);
						}
					}
					shared_this->running_ = false;
				} catch (zmq::error_t &e) {
					auto shared_this = weak_this.lock();
					if (shared_this) {
						shared_this->running_ = false;
						if (e.num() != ETERM) {
							if (shared_this->error_handler != nullptr) {
								shared_this->error_handler(e);
							}
						}
					}
				}
			});
			t.detach();

			return true;
		}

		void dispose()
		{
			disposed_ = true;
		}

		bool is_disposed()
		{
			return disposed_;
		}
	};
}
}