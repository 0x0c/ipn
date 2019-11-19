#pragma once

#include <functional>
#include <iostream>
#include <thread>

#include <msgpack.hpp>
#include <zmq.hpp>

#include "constant.hpp"
#include "packable_message.hpp"

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
			static_assert(std::is_base_of<packable_message::abstract_message_t, Request>::value, "Request not derived from packable_message");
			static_assert(std::is_base_of<packable_message::abstract_message_t, Response>::value, "Response not derived from packable_message");
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
					packable_message::unpack<Request>(msg, req);

					auto res = handler(req);
					zmq::message_t reply_msg(res.size());
					packable_message::pack(res, reply_msg);
					rep.send(reply_msg, zmq::send_flags::none);
				}
			}
		}
	};
}
}