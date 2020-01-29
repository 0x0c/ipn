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
	class service
	{
	private:
		std::string endpoint;

	public:
		service(const std::string &endpoint)
		    : endpoint(ipn::rep_endpoint(endpoint))
		{
			static_assert(std::is_base_of<google::protobuf::Message, Request>::value, "Request not derived from google::protobuf::Message");
			static_assert(std::is_base_of<google::protobuf::Message, Response>::value, "Response not derived from google::protobuf::Message");
		}

		void run(std::function<Response(Request &)> handler)
		{
			zmq::socket_t rep(*shared_ctx(), zmq::socket_type::rep);
			rep.bind(this->endpoint);

			while (true) {
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
		}
	};
}
}