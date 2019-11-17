#pragma once

#include <zmq.hpp>

namespace m2d
{
namespace ipn
{
	namespace topic
	{
		static const std::string all = "";
	}

	static zmq::context_t *shared_ctx()
	{
		static zmq::context_t ctx;
		return &ctx;
	};

	static inline const std::string broker_endpoint(const std::string &domain, const std::string &protocol = "ipc://")
	{
		std::string new_endpoint;
		new_endpoint += protocol;
		new_endpoint += domain;
		new_endpoint += ".broker";
		return new_endpoint;
	}

	static inline const std::string pub_endpoint(const std::string &endpoint, const std::string &protocol = "ipc://")
	{
		std::string new_endpoint;
		new_endpoint += protocol;
		new_endpoint += endpoint;
		new_endpoint += ".pub";
		return new_endpoint;
	}

	static inline const std::string rep_endpoint(const std::string &endpoint, const std::string &protocol = "ipc://")
	{
		std::string new_endpoint;
		new_endpoint += protocol;
		new_endpoint += endpoint;
		new_endpoint += ".rep";
		return new_endpoint;
	}
}
}