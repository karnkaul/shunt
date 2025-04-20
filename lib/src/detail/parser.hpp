#pragma once
#include <detail/token_sink.hpp>
#include <klib/visitor.hpp>
#include <shunt/result.hpp>
#include <cassert>
#include <optional>
#include <vector>

namespace shunt::detail {
class Parser {
  public:
	explicit Parser(std::vector<Token>& stack, ITokenSink& sink) : m_stack(stack), m_sink(sink) {
		m_stack.clear();
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
			m_sink.on_output(m_current);
			m_negate_operand = false;
			return;
		}

		auto const visitor = klib::Visitor{
			[this](Eof) {
				drain_stack();
				m_sink.on_output(m_current);
			},
			[this](Paren const paren) {
				switch (paren) {
				case Paren::Left: m_stack.push_back(m_current); break;
				case Paren::Right: on_paren_r(); break;
				default: throw SyntaxError{.description = "ICE"};
				}
			},
			[this](Operator const op) { apply_operator(op); },
			[this](Call) { m_stack.push_back(m_current); },
			[this](Operand) { m_sink.on_output(m_current); },
		};
		std::visit(visitor, m_current.type);
	}

	void drain_stack() {
		while (!m_stack.empty()) {
			auto const token = m_stack.back();
			m_stack.pop_back();

			if (token.is<Operator>() || token.is<Call>()) {
				m_sink.on_output(token);
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
			if (auto const* paren = token.get_if<Paren>();
				paren != nullptr && *paren == Paren::Left) {
				pop_if_func();
				return;
			}
			m_sink.on_output(token);
		}

		throw SyntaxError{
			.description = "Mismatched closing parenthesis",
			.lexeme = m_current.lexeme,
			.loc = m_current.loc,
		};
	}

	void pop_if_func() {
		if (m_stack.empty()) { return; }
		auto const token = m_stack.back();
		if (!token.is<Call>()) { return; }
		m_stack.pop_back();
		m_sink.on_output(token);
	}

	void apply_operator(Operator const op) {
		if ((!m_previous || !m_previous->is<Operand>()) && op.type() == Operator::Type::Minus) {
			m_negate_operand = true;
			return;
		}

		auto const p_current = op.precedence();
		while (!m_stack.empty()) {
			auto const token = m_stack.back();
			if (auto const* paren = token.get_if<Paren>();
				paren != nullptr && *paren == Paren::Left) {
				break;
			}
			if (auto const* bin_op = token.get_if<Operator>()) {
				auto const p_stack = bin_op->precedence();
				if (p_current > p_stack) { break; }
			}
			m_sink.on_output(token);
			m_stack.pop_back();
		}
		m_stack.push_back(m_current);
	}

	std::vector<Token>& m_stack;
	ITokenSink& m_sink;

	Token m_current{};
	std::optional<Token> m_previous{};
	bool m_negate_operand{};
};
} // namespace shunt::detail
