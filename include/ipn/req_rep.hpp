#pragma once

#include "packable_message.hpp"

namespace m2d
{
namespace ipn
{
	class response : public packable_message::abstract_message
	{
	public:
		std::string message;
		MSGPACK_DEFINE(message);

		response() {}
		response(std::string message)
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

	class request : public packable_message::abstract_message
	{
	public:
		std::string message;
		MSGPACK_DEFINE(message);
		request() {}
		request(std::string message)
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