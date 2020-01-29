#pragma once

#include <functional>
#include <thread>

#include <google/protobuf/message.h>
#include <zmq.hpp>

#include "constant.hpp"

namespace m2d
{
namespace ipn
{
	template <typename T>
	class subscriber
	{
	private:
		std::string endpoint_;

	public:
		subscriber(const std::string &endpoint)
		    : endpoint_(endpoint)
		{
			static_assert(std::is_base_of<google::protobuf::Message, T>::value, "T not derived from google::protobuf::Message");
		}

		void subscribe(const std::string &topic, std::function<void(T)> handler)
		{
			std::thread t([&] {
				zmq::socket_t sub(*shared_ctx(), zmq::socket_type::sub);
				sub.connect(pub_endpoint(this->endpoint_));
				sub.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());

				while (true) {
					zmq::message_t topic_msg, data_msg;
					sub.recv(topic_msg);
					sub.recv(data_msg);

					const std::string topic_str(static_cast<const char *>(topic_msg.data()), topic_msg.size());
					const std::string data_str(static_cast<const char *>(data_msg.data()), data_msg.size());

					T data;
					data.ParseFromString(data_str);
					handler(data);
				}
			});
			t.detach();
		}
	};
}
}