#pragma once

#include <algorithm>
#include <random>

#include <zmq.hpp>

namespace m2d
{
namespace ipn
{
	namespace topic
	{
		static const std::string all = "";
	}

	static inline const std::string unique_identifier()
	{
		std::string unique_identifier;
		static std::string chars = "abcdefghijklmnopqrstuwxyzABCDEFGHIJKLMNOPQRSTUWXYZ0123456789,./;'[]-=<>?:{}|_+";
		static std::random_device rnd;
		static std::mt19937 mt(rnd());
		static std::uniform_int_distribution<> idx(0, 32);
		for (int i = 0; i < 32; ++i) {
			unique_identifier += chars[idx(mt)];
		}
		return unique_identifier;
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