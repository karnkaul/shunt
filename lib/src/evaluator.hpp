#pragma once
#include <klib/visitor.hpp>
#include <shunt/call_table.hpp>
#include <shunt/result.hpp>
#include <shunt/token.hpp>
#include <cassert>
#include <cmath>
#include <span>
#include <vector>

namespace shunt {
class Evaluator {
  public:
	explicit Evaluator(CallTable const& call_table, std::vector<Operand>& operands)
		: m_call_table(call_table), m_operands(operands) {}

	[[nodiscard]] auto evaluate(std::span<Token const> rpn_stack) -> Result<Operand> {
		m_operands.clear();
		m_current = {};

		try {
			for (auto const& token : rpn_stack) {
				m_current = token;
				eval_current();
			}
			if (m_operands.size() != 1) { throw SyntaxError{.description = "Invalid expression"}; }
			return m_operands.front();
		} catch (SyntaxError const& error) { return std::unexpected(error); }
	}

  private:
	void eval_current() {
		auto const visitor = klib::Visitor{
			[this](Paren const /*paren*/) {
				throw SyntaxError{
					.description = "Unexpected parenthesis",
					.lexeme = m_current.lexeme,
					.loc = m_current.loc,
				};
			},
			[this](Operator const op) { apply_operator(op); },
			[this](Operand const op) { m_operands.push_back(op); },
			[this](Call const call) { apply_call(call); },
		};
		std::visit(visitor, m_current.type);
	}

	void apply_operator(Operator const op) {
		auto const operands = pop_operands<2>();
		m_operands.push_back(op.evaluate(operands[1], operands[0]));
	}

	void apply_call(Call const call) {
		auto const token = m_current;
		auto const operands = pop_operands<1>();
		auto* func = find_func(m_call_table, call);
		if (func == nullptr) {
			throw SyntaxError{
				.description = "Unrecognized call",
				.lexeme = token.lexeme,
				.loc = token.loc,
			};
		}
		m_operands.push_back(func(operands[0]));
	}

	template <std::size_t Count>
	[[nodiscard]] auto pop_operands() -> std::array<Operand, Count> {
		if (m_operands.size() < Count) {
			throw SyntaxError{
				.description = "Insufficient operands",
				.lexeme = m_current.lexeme,
				.loc = m_current.loc,
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
	Token m_current{};
};
} // namespace shunt
