#pragma once
#include <detail/token_sink.hpp>
#include <vector>

namespace shunt::detail {
class RpnSink : public ITokenSink {
  public:
	explicit RpnSink(std::vector<Token>& tokens) : m_tokens(tokens) {}

	void on_operator(Token const& token) final { m_tokens.push_back(token); }
	void on_operand(Token const& token) final { m_tokens.push_back(token); }
	void on_call(Token const& token) final { m_tokens.push_back(token); }
	void on_eof(Token const& token) final { m_tokens.push_back(token); }

  private:
	std::vector<Token>& m_tokens;
};
} // namespace shunt::detail
