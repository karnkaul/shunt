#pragma once
#include <detail/token_sink.hpp>
#include <vector>

namespace shunt::detail {
struct RpnSink : ITokenSink {
	void on_output(Token const& token) final { tokens.push_back(token); }

	std::vector<Token> tokens{};
};
} // namespace shunt::detail
