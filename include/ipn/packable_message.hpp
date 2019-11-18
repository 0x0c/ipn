#pragma once

#include <msgpack.hpp>
#include <zmq.hpp>

namespace m2d
{
namespace ipn
{
	namespace packable_message
	{
		class abstract_message_t
		{
			virtual void *data() const = 0;
			virtual size_t size() const = 0;
		};

		void _pack(const msgpack::sbuffer &packed_data, zmq::message_t &msg)
		{
			std::memcpy(msg.data(), packed_data.data(), packed_data.size());
		}

		template <typename T>
		void pack(const T &data, zmq::message_t &msg)
		{
			msgpack::sbuffer packed;
			msgpack::pack(&packed, data);
			_pack(packed, msg);
		}

		msgpack::unpacked _unpack(zmq::message_t &msg)
		{
			msgpack::unpacked unpacked_body;
			msgpack::unpack(unpacked_body, static_cast<const char *>(msg.data()), msg.size());
			return unpacked_body;
		}

		template <typename T>
		void unpack(zmq::message_t &msg, T &data)
		{
			auto unpacked_body = _unpack(msg);
			unpacked_body.get().convert(data);
		}
	}
}
}