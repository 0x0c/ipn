#pragma once

#include <functional>
#include <iostream>
#include <tuple>

#include <boost/optional.hpp>
#include <google/protobuf/message.h>
#include <msgpack.hpp>
#include <zmq.hpp>

#include "constant.hpp"

namespace m2d
{
namespace ipn
{
	class error_t
	{
	public:
		int err_no = 0;
		std::string description = "";
	};

	template <typename Response>
	class result_t
	{
	public:
		boost::optional<Response> response;
		boost::optional<error_t> error;
		result_t(Response response)
		    : response(response)
		    , error(boost::none)
		{
			static_assert(std::is_base_of<google::protobuf::Message, Response>::value, "Response not derived from google::protobuf::Message");
		}

		result_t(error_t error)
		    : response(boost::none)
		    , error(error)
		{
		}
	};

	template <typename Request, typename Response>
	class client
	{
	private:
		std::string endpoint_;

		static zmq::socket_t create_socket(zmq::context_t &context, std::string endpoint)
		{
			zmq::socket_t client(context, ZMQ_REQ);
			client.connect(endpoint);

			//  Configure socket to not wait at close time
			int linger = 0;
			client.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
			return client;
		}

		zmq::detail::recv_result_t receive(zmq::socket_t &client, Response &data)
		{
			zmq::message_t data_msg;
			auto result = client.recv(data_msg);
			auto parsed = false;
			if (result.value()) {
				const std::string data_str(static_cast<const char *>(data_msg.data()), data_msg.size());
				parsed = data.ParseFromString(data_str);
			}

			return (result.value() > 0) & parsed;
		}

	public:
		std::function<void(zmq::error_t &e)> error_handler = nullptr;

		client(const std::string &endpoint)
		    : endpoint_(ipn::rep_endpoint(endpoint))
		{
			static_assert(std::is_base_of<google::protobuf::Message, Response>::value, "Response not derived from google::protobuf::Message");
			static_assert(std::is_base_of<google::protobuf::Message, Request>::value, "Request not derived from google::protobuf::Message");
		}

		result_t<Response> send(const Request &request, int timeout_msec = 500, int retry_count = 1)
		{
			zmq::socket_t client = create_socket(*shared_ctx(), endpoint_);
			// pack to zmq::message_t
			auto serialized_string = request.SerializeAsString();
			auto size = serialized_string.size() * sizeof(std::string::value_type);
			zmq::message_t request_msg(size);
			std::memcpy(request_msg.data(), serialized_string.c_str(), size);

			try {
				int retries_left = retry_count;
				client.send(request_msg, zmq::send_flags::none);
				while (true) {
					//  Poll socket for a reply, with timeout
					zmq::pollitem_t items[] = {
						{ static_cast<void *>(client), 0, ZMQ_POLLIN, 0 }
					};
					zmq::poll(&items[0], 1, timeout_msec);

					//  If we got a reply, process it
					if (items[0].revents & ZMQ_POLLIN) {
						Response res;
						auto result = receive(client, res);
						error_t error;
						if (result.value() == 0) {
							error.err_no = -1;
							error.description = "malformed reply from server.";
							return result_t<Response>(error);
						}

						return result_t<Response>(res);
					}
					else if (--retries_left == 0) {
						error_t error;
						error.err_no = -2;
						error.description = "server seems to be offline, abandoning.";

						return result_t<Response>(error);
					}
					else {
						client = create_socket(*shared_ctx(), endpoint_);
						client.send(request_msg, zmq::send_flags::none);
					}
				}
			} catch (zmq::error_t &e) {
				if (e.num() != ETERM) {
					if (error_handler != nullptr) {
						error_handler(e);
					}
				}
				error_t error;
				error.err_no = e.num();
				error.description = e.what();
				return result_t<Response>(error);
			}
		}
	};
}
}