#pragma once
#include <detail/token_sink.hpp>
#include <vector>

namespace shunt::detail {
class RpnSink : public ITokenSink {
  public:
	explicit RpnSink(std::vector<Token>& output) : m_output(output) { m_output.clear(); }

	void on_operator(Token const& token) final { m_output.push_back(token); }
	void on_operand(Token const& token) final { m_output.push_back(token); }
	void on_call(Token const& token) final { m_output.push_back(token); }
	void on_eof(Token const& token) final { m_output.push_back(token); }

  private:
	std::vector<Token>& m_output;
};
} // namespace shunt::detail
