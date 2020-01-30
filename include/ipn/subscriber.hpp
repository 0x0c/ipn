#pragma once

#include <algorithm>
#include <functional>
#include <random>
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
		    : endpoint_(endpoint)
		{
			static_assert(std::is_base_of<google::protobuf::Message, T>::value, "T not derived from google::protobuf::Message");
		}

		std::string subscribe(const std::string &topic, std::function<void(T)> handler)
		{
			std::string hash;
			static std::string chars = "abcdefghijklmnopqrstuwxyzABCDEFGHIJKLMNOPQRSTUWXYZ0123456789,./;'[]-=<>?:{}|_+";
			static std::random_device rnd;
			static std::mt19937 mt(rnd());
			static std::uniform_int_distribution<> idx(0, 32);
			for (int i = 0; i < 32; ++i) {
				hash += chars[idx(mt)];
			}
			disposed_.insert(std::make_pair(hash, false));

			std::function<void(zmq::error_t & e)> e_handler = error_handler;
			std::thread t([=] {
				std::weak_ptr<subscriber<T>> weak_this = this->shared_from_this();
				zmq::socket_t sub(*shared_ctx(), zmq::socket_type::sub);
				sub.connect(pub_endpoint(this->endpoint_));
				sub.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());

				try {
					while (weak_this.expired() == false && this->is_disposed(hash) == false) {
						zmq::message_t topic_msg, data_msg;
						sub.recv(topic_msg);
						sub.recv(data_msg);

						if (weak_this.expired() || this->is_disposed(hash)) {
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
					std::cout << "dispose or expired" << std::endl;
				} catch (zmq::error_t &e) {
					if (e.num() != ETERM) {
						if (e_handler != nullptr) {
							e_handler(e);
						}
					}
				}
			});
			t.detach();

			return hash;
		}

		void dispose(std::string hash)
		{
			auto it = disposed_.find(hash);
			if (it != disposed_.end()) {
				disposed_.erase(it);
				disposed_.insert(std::make_pair(hash, true));
			}
		}

		void dispose_all()
		{
			for (auto it = disposed_.begin(); it != disposed_.end(); ++it) {
				auto hash = it->first;
				disposed_.erase(it);
				disposed_.insert(std::make_pair(hash, true));
			}
		}

		bool is_disposed(std::string hash)
		{
			auto it = disposed_.find(hash);
			if (it != disposed_.end()) {
				return it->second;
			}
			return true;
		}
	};
}
}
