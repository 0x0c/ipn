#pragma once

#include <msgpack.hpp>
#include <zmq.hpp>

namespace m2d
{
namespace ipn
{
	namespace packable_message
	{
		class abstract_message
		{
			virtual void *data() const = 0;
			virtual size_t size() const = 0;
		};

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
	}
}
}