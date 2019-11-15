#pragma once

#include <functional>

#include <msgpack.hpp>
#include <zmq.hpp>

#include "constant.hpp"
#include "packable_message.hpp"

namespace m2d
{
namespace ipn
{
	template <typename T>
	class publisher
	{
	private:
		zmq::socket_t pub;

	public:
		publisher(const std::string &endpoint)
		{
			pub = zmq::socket_t(*shared_ctx(), zmq::socket_type::pub);
			pub.bind(pub_endpoint(endpoint));
		}

		void send(const std::string &topic, const T &data)
		{
			zmq::message_t topic_msg(topic.size());
			packable_message::pack(topic, topic_msg);

			zmq::message_t data_msg(data.size());
			packable_message::pack(data, data_msg);

			pub.send(topic_msg, zmq::send_flags::sndmore);
			pub.send(data_msg, zmq::send_flags::none);
		}
	};
}
}