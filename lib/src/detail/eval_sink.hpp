#pragma once
#include <detail/token_sink.hpp>
#include <klib/visitor.hpp>
#include <shunt/call_table.hpp>
#include <shunt/result.hpp>
#include <shunt/syntax_error.hpp>
#include <vector>

namespace shunt::detail {
class EvalSink : public ITokenSink {
  public:
	explicit EvalSink(CallTable const& call_table, std::vector<Operand>& operands)
		: m_call_table(call_table), m_operands(operands) {
		m_operands.clear();
	}

	[[nodiscard]] auto get_value() -> Result<Operand> {
		if (m_operands.size() != 1) {
			return std::unexpected(SyntaxError{.description = "Invalid expression"});
		}
		return m_operands.front();
	}

	[[nodiscard]] auto is_defined(Call call) const -> bool final {
		return find_func(m_call_table, call) != nullptr;
	}

	void on_operator(Token const& token) final {
		auto const& op = token.get<Operator>();
		auto const operands = pop_operands<2>(token);
		m_operands.push_back(op.evaluate(operands[1], operands[0]));
	}

	void on_operand(Token const& token) final { m_operands.push_back(token.get<Operand>()); }

	void on_call(Token const& token) final {
		auto* func = find_func(m_call_table, token.get<Call>());
		assert(func);
		auto const operands = pop_operands<1>(token);
		m_operands.push_back(func(operands[0]));
	}

	void on_eof(Token const& /*token*/) final {}

  private:
	template <std::size_t Count>
	[[nodiscard]] auto pop_operands(Token const& token) -> std::array<Operand, Count> {
		if (m_operands.size() < Count) {
			throw SyntaxError{
				.description = "Insufficient operands",
				.lexeme = token.lexeme,
				.loc = token.loc,
			};
		}
		auto ret = std::array<Operand, Count>{};
		for (auto& op : ret) {
			op = m_operands.back();
			m_operands.pop_back();
		}
		return ret;
	}

	CallTable const& m_call_table;
	std::vector<Operand>& m_operands;
};
} // namespace shunt::detail
