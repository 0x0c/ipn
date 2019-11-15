#pragma once

#include <functional>
#include <iostream>
#include <tuple>

#include <msgpack.hpp>
#include <zmq.hpp>

#include "constant.hpp"
#include "packable_message.hpp"

namespace m2d
{
namespace ipn
{
	struct error_t
	{
		int error_no = 0;
		std::string description = "";
	};

	template <typename Response>
	class result_t
	{
	public:
		Response res;
		error_t err;
		result_t(Response res, error_t err)
		    : res(res)
		    , err(err)
		{
		}
	};

	template <typename Request, typename Response>
	class client
	{
	private:
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
			if (result.value()) {
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

		result_t<Response> send(const Request &request, int timeout_msec = 500, int retry_count = 1)
		{
			zmq::socket_t client = create_socket(*shared_ctx(), endpoint);
			zmq::message_t request_msg(request.size());
			packable_message::pack(request, request_msg);

			Request r;
			packable_message::unpack(request_msg, r);

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
					//  We got a reply from the server, must match sequence
					Response res;
					auto result = receive(client, res);
					error_t error;
					if (result.value() == 0) {
						error.error_no = -1;
						error.description = "malformed reply from server.";
					}

					return result_t<Response>(res, error);
				}
				else if (--retries_left == 0) {

					error_t error;
					error.error_no = -2;
					error.description = "server seems to be offline, abandoning.";

					Response res;
					return result_t<Response>(res, error);
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