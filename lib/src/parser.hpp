#pragma once
#include <klib/visitor.hpp>
#include <shunt/result.hpp>
#include <shunt/token.hpp>
#include <cassert>
#include <optional>
#include <span>
#include <vector>

namespace shunt {
class Parser {
  public:
	explicit Parser(std::vector<Token>& out, std::vector<Token>& stack)
		: m_stack(stack), m_output(out) {}

	[[nodiscard]] auto parse(std::span<Token const> tokens) -> Result<void> {
		m_stack.clear();
		m_output.clear();
		m_current = {};

		try {
			for (auto const token : tokens) {
				m_current = token;
				parse_current();
				m_previous = m_current;
			}
			if (!m_stack.empty()) {
				throw SyntaxError{
					.description = "Unexpected token",
					.lexeme = m_stack.front().lexeme,
					.loc = m_stack.front().loc,
				};
			}
		} catch (SyntaxError const& error) { return std::unexpected(error); }
		return {};
	}

	[[nodiscard]] auto parse_next(Token const& token) -> Result<void> {
		try {
			m_current = token;
			parse_current();
			m_previous = m_current;
		} catch (SyntaxError const& error) { return std::unexpected(error); }
		return {};
	}

  private:
	void parse_current() {
		if (m_negate_operand) {
			auto const* operand = m_current.get_if<Operand>();
			if (operand == nullptr) {
				throw SyntaxError{
					.description = "Expected operand",
					.lexeme = m_current.lexeme,
					.loc = m_current.loc,
				};
			}
			m_current.type = -*operand;
			m_output.push_back(m_current);
			m_negate_operand = false;
			return;
		}

		auto const visitor = klib::Visitor{
			[this](Eof) { drain_stack(); },
			[this](Paren const paren) {
				switch (paren) {
				case Paren::Left: m_stack.push_back(m_current); break;
				case Paren::Right: on_paren_r(); break;
				default: throw SyntaxError{.description = "ICE"};
				}
			},
			[this](Operator const op) { apply_operator(op); },
			[this](Call) { m_stack.push_back(m_current); },
			[this](Operand) { m_output.push_back(m_current); },
		};
		std::visit(visitor, m_current.type);
	}

	void drain_stack() {
		while (!m_stack.empty()) {
			auto const token = m_stack.back();
			m_stack.pop_back();

			if (token.is<Operator>() || token.is<Call>()) {
				m_output.push_back(token);
				continue;
			}

			throw SyntaxError{
				.description =
					token.is<Paren>() ? "Mismatched opening parenthesis" : "Unexpected token",
				.lexeme = token.lexeme,
				.loc = token.loc,
			};
		}
	}

	void on_paren_r() {
		while (!m_stack.empty()) {
			auto const token = m_stack.back();
			m_stack.pop_back();
			if (token.is<Paren>()) {
				assert(token.get<Paren>() == Paren::Left);
				pop_if_call();
				return;
			}
			m_output.push_back(token);
		}

		throw SyntaxError{
			.description = "Mismatched closing parenthesis",
			.lexeme = m_current.lexeme,
			.loc = m_current.loc,
		};
	}

	void pop_if_call() {
		if (m_stack.empty()) { return; }
		auto const token = m_stack.back();
		if (!token.is<Call>()) { return; }
		m_stack.pop_back();
		m_output.push_back(token);
	}

	[[nodiscard]] auto can_unary_minus() const -> bool {
		if (!m_previous) { return true; }
		if (m_previous->is<Operator>()) { return true; }
		if (auto const* paren = m_previous->get_if<Paren>()) { return *paren == Paren::Left; }
		return false;
	}

	void apply_operator(Operator const op) {
		if (op.type() == Operator::Type::Minus && can_unary_minus()) {
			m_negate_operand = true;
			return;
		}

		auto const p_current = op.precedence();
		while (!m_stack.empty()) {
			auto const token = m_stack.back();
			if (token.is<Paren>()) {
				assert(token.get<Paren>() == Paren::Left);
				break;
			}
			if (auto const* bin_op = token.get_if<Operator>()) {
				auto const p_stack = bin_op->precedence();
				if (p_current > p_stack) { break; }
			}
			m_output.push_back(token);
			m_stack.pop_back();
		}
		m_stack.push_back(m_current);
	}

	std::vector<Token>& m_stack;
	std::vector<Token>& m_output;

	Token m_current{};
	std::optional<Token> m_previous{};
	bool m_negate_operand{};
};
} // namespace shunt
