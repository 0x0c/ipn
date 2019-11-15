#pragma once

class message
{
public:
	std::string text;
	MSGPACK_DEFINE(text);

	message() {}
	message(const std::string &text)
	    : text(text)
	{
	}

	msgpack::sbuffer packed_data() const
	{
		msgpack::sbuffer packed;
		msgpack::pack(packed, *this);
		return packed;
	}

	void *data() const
	{
		return packed_data().data();
	}

	size_t size() const
	{
		auto data = packed_data();
		return data.size();
	}
};