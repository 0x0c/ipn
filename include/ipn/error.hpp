#pragma once

namespace m2d
{
namespace ipn
{
	class error_t
	{
	public:
		enum error_no
		{
			no_error,
			parse_error,
			invalid_response,
			catch_zmq_error
		};

		int err_no = 0;
		zmq::error_t zmq_error;
		std::string description = "";
	};
}
}