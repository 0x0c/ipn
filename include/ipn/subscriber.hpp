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
	class subscriber : public std::enable_shared_from_this<subscriber<T>>
	{
	private:
		std::string endpoint_;
		std::map<std::string, bool> disposed_;

	public:
		std::function<void(zmq::error_t &e)> error_handler;

		subscriber(const std::string &endpoint)
		    : endpoint_(ipn::pub_endpoint(endpoint))
		{
			static_assert(std::is_base_of<google::protobuf::Message, T>::value, "T not derived from google::protobuf::Message");
		}

		std::string subscribe(const std::string &topic, std::function<void(T)> handler)
		{
			auto unique_identifier = ipn::unique_identifier();
			disposed_.insert(std::make_pair(unique_identifier, false));

			std::function<void(zmq::error_t & e)> e_handler = error_handler;
			std::weak_ptr<subscriber<T>> weak_this = this->shared_from_this();
			std::thread t([=] {
				auto shared_this = weak_this.lock();
				if (!shared_this) {
					return;
				}

				try {
					zmq::socket_t sub(*shared_ctx(), zmq::socket_type::sub);
					sub.connect(shared_this->endpoint_);
					sub.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
					while (shared_this->is_disposed(unique_identifier) == false) {
						zmq::message_t topic_msg, data_msg;
						sub.recv(topic_msg);
						sub.recv(data_msg);

						if (shared_this->is_disposed(unique_identifier)) {
							break;
						}

						const std::string topic_str(static_cast<const char *>(topic_msg.data()), topic_msg.size());
						const std::string data_str(static_cast<const char *>(data_msg.data()), data_msg.size());

						T data;
						data.ParseFromString(data_str);
						if (handler != nullptr) {
							handler(data);
						}
					}
				} catch (zmq::error_t &e) {
					if (e.num() != ETERM) {
						if (e_handler != nullptr) {
							e_handler(e);
						}
					}
				}
			});
			t.detach();

			return unique_identifier;
		}

		void dispose(std::string unique_identifier)
		{
			auto it = disposed_.find(unique_identifier);
			if (it != disposed_.end()) {
				disposed_.erase(it);
				disposed_.insert(std::make_pair(unique_identifier, true));
			}
		}

		void dispose_all()
		{
			for (auto it = disposed_.begin(); it != disposed_.end(); ++it) {
				auto unique_identifier = it->first;
				disposed_.erase(it);
				disposed_.insert(std::make_pair(unique_identifier, true));
			}
		}

		bool is_disposed(std::string unique_identifier)
		{
			auto it = disposed_.find(unique_identifier);
			if (it != disposed_.end()) {
				return it->second;
			}
			return true;
		}
	};
}
}
