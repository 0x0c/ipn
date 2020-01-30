#pragma once

#include <functional>

#include <google/protobuf/message.h>
#include <msgpack.hpp>
#include <zmq.hpp>

#include "constant.hpp"

namespace m2d
{
namespace ipn
{
	template <typename T>
	class publisher : public std::enable_shared_from_this<publisher<T>>
	{
	private:
		zmq::socket_t pub;

	public:
		publisher(const std::string &endpoint)
		{
			static_assert(std::is_base_of<google::protobuf::Message, T>::value, "T not derived from google::protobuf::Message");
			pub = zmq::socket_t(*shared_ctx(), zmq::socket_type::pub);
			pub.bind(pub_endpoint(endpoint));
		}

		void send(const std::string &topic, const T &data)
		{
			zmq::message_t topic_msg(topic.size());
			std::memcpy(topic_msg.data(), topic.c_str(), topic.size());

			// pack to zmq::message_t
			auto serialized_string = data.SerializeAsString();
			auto size = serialized_string.size() * sizeof(std::string::value_type);
			zmq::message_t data_msg(size);
			std::memcpy(data_msg.data(), serialized_string.c_str(), size);

			pub.send(topic_msg, zmq::send_flags::sndmore);
			pub.send(data_msg, zmq::send_flags::none);
		}
	};
}
}