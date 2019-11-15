#pragma once

#include "ipn/client.hpp"
#include "ipn/constant.hpp"
#include "ipn/packable_message.hpp"
#include "ipn/publisher.hpp"
#include "ipn/req_rep.hpp"
#include "ipn/service.hpp"
#include "ipn/subscriber.hpp"

namespace m2d
{
namespace ipn
{

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

} // namespace ipn
} // namespace m2d
