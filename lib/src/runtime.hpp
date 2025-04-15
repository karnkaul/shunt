#pragma once
#include <eval.hpp>
#include <shunt/result.hpp>
#include <shunt/token.hpp>
#include <shunt/visitor.hpp>
#include <cassert>
#include <cmath>
#include <span>
#include <vector>

namespace shunt {
class Runtime {
  public:
	explicit Runtime(Eval const& eval) : m_eval(&eval) {}

	[[nodiscard]] auto evaluate(std::span<RpnToken const> rpn_stack) -> Result<Operand> {
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
		auto const visitor = Visitor{
			[this](BinaryOp const op) { apply_bin_op(op); },
			[this](Operand const op) { m_operands.push_back(op); },
			[this](Call const call) { apply_call(call); },
		};
		std::visit(visitor, m_current.term);
	}

	void apply_bin_op(BinaryOp const op) {
		auto const operands = pop_operands<2>();
		auto const args = BinOpArgs{
			.op = op,
			.a = operands[0],
			.b = operands[1],
		};
		auto result = double{};
		m_eval->evaluate(args, result);
		m_operands.push_back(result);
	}

	void apply_call(Call const call) {
		auto const token = m_current;
		auto const operand = pop_operands<1>();
		auto const args = CallArgs{
			.call = call,
			.x = operand[0],
		};
		auto result = double{};
		if (!m_eval->evaluate(args, result)) {
			throw SyntaxError{
				.description = "Unrecognized call",
				.lexeme = token.lexeme,
				.loc = token.loc,
			};
		}
		m_operands.push_back(result);
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

	[[nodiscard]] static auto compute(Operand const lhs, BinaryOp const op, Operand const rhs)
		-> Operand {
		switch (op) {
		case BinaryOp::Caret: return std::pow(lhs, rhs);
		case BinaryOp::Plus: return lhs + rhs;
		case BinaryOp::Minus: return lhs - rhs;
		case BinaryOp::Star: return lhs * rhs;
		case BinaryOp::Slash: return lhs / rhs;
		default: return 0.0;
		}
	}

	Eval const* m_eval{};
	std::vector<Operand> m_operands{};
	RpnToken m_current{};
};
} // namespace shunt
