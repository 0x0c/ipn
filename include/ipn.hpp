#pragma once

#include <functional>
#include <thread>

#include <msgpack.hpp>
#include <zmq.hpp>

namespace m2d
{
namespace ipn
{
	namespace topic
	{
		static const std::string all = "";
	}

	template <typename T>
	void pack(const T &data, zmq::message_t &msg)
	{
		msgpack::sbuffer packed;
		msgpack::pack(&packed, data);
		std::memcpy(msg.data(), data.data(), data.size());
	}

	template <typename T>
	void unpack(zmq::message_t &msg, T &data)
	{
		msgpack::unpacked unpacked_body;
		msgpack::unpack(unpacked_body, static_cast<const char *>(msg.data()), msg.size());
		unpacked_body.get().convert(data);
	}

	static zmq::context_t *shared_ctx()
	{
		static zmq::context_t ctx;
		return &ctx;
	};

	static inline const std::string broaker_endpoint(const std::string &domain, const std::string &protocol = "ipc://")
	{
		std::string new_endpoint;
		new_endpoint += protocol;
		new_endpoint += domain;
		new_endpoint += ".broaker";
		return new_endpoint;
	}

	static inline const std::string pub_endpoint(const std::string &endpoint, const std::string &protocol = "ipc://")
	{
		std::string new_endpoint;
		new_endpoint += protocol;
		new_endpoint += endpoint;
		new_endpoint += ".pub";
		return new_endpoint;
	}

	static inline const std::string rep_endpoint(const std::string &endpoint, const std::string &protocol = "ipc://")
	{
		std::string new_endpoint;
		new_endpoint += protocol;
		new_endpoint += endpoint;
		new_endpoint += ".rep";
		return new_endpoint;
	}

	// class broaker
	// {
	// private:
	// 	zmq::socket_t pub(shared_ctx, zmq::socket_type::xpub);
	// 	zmq::socket_t sub(shared_ctx, zmq::socket_type::xsub);

	// public:
	// 	broaker(const std::string &domain)
	// 	{
	// 		pub.bind(broaker_endpoint(domain));
	// 		sub.bind(broaker_endpoint(domain));
	// 	}

	// 	void run()
	// 	{
	// 		while (1) {
	// 			while (1) {
	// 				zmq::message_t message;
	// 				int more;
	// 				size_t more_size = sizeof(more);

	// 				//  Process all parts of the message
	// 				frontend.recv(&message);
	// 				frontend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
	// 				backend.send(message, more ? ZMQ_SNDMORE : 0);
	// 				if (!more)
	// 					break; //  Last message part
	// 			}
	// 		}
	// 	}
	// }

	class packable_object
	{
	public:
		std::string name;
		MSGPACK_DEFINE(name);
		packable_object() {}

		msgpack::sbuffer packed_data() const
		{
			msgpack::sbuffer packed;
			msgpack::pack(packed, *this);
			return packed;
		};

		void *data() const
		{
			return packed_data().data();
		}

		size_t size() const
		{
			return packed_data().size();
		}
	};

	class response : public packable_object
	{
	public:
		std::string message;
		MSGPACK_DEFINE(message);
		response()
		    : packable_object()
		{
		}

		response(const std::string &message)
		    : packable_object()
		    , message(message)
		{
		}
	};

	class request : public packable_object
	{
	public:
		std::string message;
		MSGPACK_DEFINE(message);
		request()
		    : packable_object()
		{
		}

		request(const std::string &message)
		    : packable_object()
		    , message(message)
		{
		}
	};

	template <typename Request, typename Response>
	class client
	{
	private:
		zmq::socket_t req;

	public:
		client(const std::string &endpoint)
		{
			zmq::context_t ctx;
			req = zmq::socket_t(*shared_ctx(), zmq::socket_type::req);
			req.connect(ipn::rep_endpoint(endpoint));
			req.setsockopt(ZMQ_RCVTIMEO, 500);
		}

		void send(const Request &request)
		{
			zmq::message_t request_msg(request.size());
			pack(request, request_msg);
			req.send(request_msg, zmq::send_flags::none);
		}

		void send(const Request &data, std::function<void(Response)> handler)
		{
			send(data);
			Response res;
			receive(res);
			handler(res);
		}

        zmq::detail::recv_result_t receive(Response &data)
		{
			zmq::message_t data_msg;
			auto result = req.recv(data_msg);
			unpack<Response>(data_msg, data);
			return result;
		}
	};

	template <typename Request, typename Response>
	class service
	{
	private:
		std::string endpoint_;

	public:
		service(const std::string &endpoint)
		    : endpoint_(endpoint)
		{
		}

		void run(std::function<Response(Request)> handler)
		{
			zmq::socket_t rep(*shared_ctx(), zmq::socket_type::rep);
			rep.bind(rep_endpoint(this->endpoint_));

			while (true) {
				zmq::message_t msg;
				auto result = rep.recv(msg);
				if (result) {
					Request req;
					unpack<Request>(msg, req);

					auto res = handler(req);
					zmq::message_t reply_msg;
					pack(res, reply_msg);
					rep.send(reply_msg, zmq::send_flags::none);
				}
			}
		}

		void async_run(std::function<Response(Request)> handler)
		{
			std::thread t([&] {
				this->run(handler);
			});
			t.detach();
		}
	};

	template <typename T>
	class publisher
	{
	private:
		zmq::socket_t *pub;
		std::string endpoint_;
		service<request, response> *s;

	public:
		publisher(const std::string &endpoint)
		{
			pub = new zmq::socket_t(*shared_ctx(), zmq::socket_type::pub);
			pub->bind(pub_endpoint(endpoint));

			s = new service<request, response>(endpoint);
			s->async_run([](request req) {
				response res("available");
				return res;
			});

			// std::thread t([&] {
			// 	zmq::socket_t rep(*shared_ctx(), zmq::socket_type::rep);
			// 	rep.bind(rep_endpoint(this->endpoint_));

			// 	while (true) {
			// 		zmq::message_t msg;
			// 		auto result = rep.recv(&msg);
			// 		if (result) {
			// 			auto res = response("available");
			// 			zmq::message_t reply;
			// 			pack(res, reply);
			// 			rep.send(reply, zmq::send_flags::none);
			// 		}
			// 	}
			// });
			// t.detach();
		}

		~publisher()
		{
			delete pub;
			delete s;
		}

		void send(const std::string &topic, const T &data)
		{
			zmq::message_t topic_msg(topic.size());
			pack(topic, topic_msg);

			zmq::message_t data_msg(data.size());
			pack(data, data_msg);

			pub->send(topic_msg, zmq::send_flags::sndmore);
			pub->send(data_msg, zmq::send_flags::none);
		}
	};

	template <typename T>
	class subscriber
	{
		using monitering_client = client<request, response>;

	private:
		std::string endpoint_;
		monitering_client *c;

	public:
		subscriber(const std::string &endpoint)
		    : endpoint_(endpoint)
		{
			c = new monitering_client(endpoint);
		}

		~subscriber()
		{
			delete c;
		}

		bool is_publisher_available()
		{
			auto ping = request("ping");
			c->send(ping);

			response res;
			try {
				c->receive(res);
				return true;
			} catch (zmq::error_t e) {
				return false;
			}
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

					const std::string topic(static_cast<const char *>(topic_msg.data()), topic_msg.size());

					T data;
					unpack<T>(data_msg, data);
					handler(data);
				}
			});
			t.detach();
		}
	};
} // namespace ipn
} // namespace m2d
