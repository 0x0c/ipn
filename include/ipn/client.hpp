#pragma once

#include <functional>
#include <iostream>

#include <msgpack.hpp>
#include <zmq.hpp>

#include "constant.hpp"
#include "packable_message.hpp"

namespace m2d
{
namespace ipn
{
	template <typename Request, typename Response>
	class client
	{
	private:
		static zmq::socket_t create_socket(zmq::context_t &context, std::string endpoint)
		{
			std::cout << "I: connecting to server..." << std::endl;
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
			if (result.value()) {
				std::cout << "valid" << std::endl;
				packable_message::unpack<Response>(data_msg, data);
			}

			return result;
		}

	public:
		std::string endpoint;
		client(const std::string &endpoint)
		    : endpoint(ipn::rep_endpoint(endpoint))
		{
		}

		void send(const Request &request, std::function<void(Response, bool)> handler, int timeout_msec = 500, int retry_count = 1)
		{
			zmq::socket_t client = create_socket(*shared_ctx(), endpoint);
			zmq::message_t request_msg(request.size());
			packable_message::pack(request, request_msg);

			Request r;
			packable_message::unpack(request_msg, r);

			int retries_left = retry_count;
			client.send(request_msg, zmq::send_flags::none);

			bool expect_reply = true;
			while (expect_reply) {
				//  Poll socket for a reply, with timeout
				zmq::pollitem_t items[] = {
					{ static_cast<void *>(client), 0, ZMQ_POLLIN, 0 }
				};
				zmq::poll(&items[0], 1, timeout_msec);

				//  If we got a reply, process it
				if (items[0].revents & ZMQ_POLLIN) {
					//  We got a reply from the server, must match sequence
					Response res;
					auto result = receive(client, res);
					handler(res, result.value() > 0);
					expect_reply = false;
					break;
				}
				else if (--retries_left == 0) {
					std::cout << "E: server seems to be offline, abandoning" << std::endl;
					expect_reply = false;
					break;
				}
				else {
					std::cout << "W: no response from server, retrying..." << std::endl;
					//  Old socket will be confused; close it and open a new one
					client = create_socket(*shared_ctx(), endpoint);

					//  Send request again, on new socket
					client.send(request_msg, zmq::send_flags::none);
				}
			}
		}
	};
}
}