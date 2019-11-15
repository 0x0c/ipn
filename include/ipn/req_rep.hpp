#pragma once

#include "packable_message.hpp"

namespace m2d
{
namespace ipn
{
	class simple_response : public packable_message::abstract_message
	{
	public:
		std::string message;
		MSGPACK_DEFINE(message);

		simple_response() {}
		simple_response(std::string message)
		    : message(message)
		{
		}

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

	class simple_request : public packable_message::abstract_message
	{
	public:
		std::string message;
		MSGPACK_DEFINE(message);
		simple_request() {}
		simple_request(std::string message)
		    : message(message)
		{
		}

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
}
}