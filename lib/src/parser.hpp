#pragma once
#include <shunt/result.hpp>
#include <shunt/token.hpp>
#include <shunt/visitor.hpp>
#include <cassert>
#include <span>
#include <vector>

namespace shunt {
class Parser {
  public:
	auto parse(std::span<Token const> tokens) -> Result<std::vector<RpnToken>> {
		m_stack.clear();
		m_output.clear();
		m_current = {};

		try {
			for (auto const token : tokens) {
				m_current = token;
				parse_current();
			}
			while (!m_stack.empty()) {
				auto const token = m_stack.back();
				m_stack.pop_back();

				auto const visitor = Visitor{
					[token](Paren const /*p*/) {
						throw SyntaxError{
							.description = "Mismatched opening parenthesis",
							.lexeme = token.lexeme,
							.loc = token.loc,
						};
					},
					[this, token](BinaryOp const /*op*/) { m_output.push_back(cast(token)); },
					[this, token](Call const /*func*/) { m_output.push_back(cast(token)); },
					[](Operand const /*op*/) { throw SyntaxError{.description = "ICE"}; },
				};
				std::visit(visitor, token.term);
			}
		} catch (SyntaxError const& error) { return std::unexpected(error); }
		return std::move(m_output);
	}

  private:
	static auto cast(Term const& term) -> RpnTerm {
		auto const visitor = Visitor{
			[](Paren const /*p*/) -> RpnTerm { throw SyntaxError{.description = "ICE"}; },
			[](auto const& t) { return RpnTerm{t}; },
		};
		return std::visit(visitor, term);
	}

	static auto cast(Token const& token) -> RpnToken {
		return RpnToken{
			.term = cast(token.term),
			.lexeme = token.lexeme,
			.loc = token.loc,
		};
	}

	void parse_current() {
		auto const visitor = Visitor{
			[this](Paren const paren) {
				switch (paren) {
				case Paren::Left: m_stack.push_back(m_current); break;
				case Paren::Right: on_paren_r(); break;
				}
			},
			[this](BinaryOp const /*op*/) { apply_bin_op(); },
			[this](Call const /*func*/) { m_stack.push_back(m_current); },
			[this](Operand const /*op*/) { m_output.push_back(cast(m_current)); },
		};
		std::visit(visitor, m_current.term);
	}

	void on_paren_r() {
		while (!m_stack.empty()) {
			auto const token = m_stack.back();
			m_stack.pop_back();
			if (auto const* paren = std::get_if<Paren>(&token.term);
				paren != nullptr && *paren == Paren::Left) {
				pop_if_func();
				return;
			}
			m_output.push_back(cast(token));
		}

		throw SyntaxError{
			.description = "Mismatched closing parenthesis",
			.lexeme = m_current.lexeme,
			.loc = m_current.loc,
		};
	}

	void pop_if_func() {
		if (m_stack.empty()) { return; }
		auto const op = m_stack.back();
		if (!std::holds_alternative<Call>(op.term)) { return; }
		m_stack.pop_back();
		m_output.push_back(cast(op));
	}

	void apply_bin_op() {
		auto const op = std::get<BinaryOp>(m_current.term);
		auto const p_current = bin_op_precedence_v.at(op);
		while (!m_stack.empty()) {
			auto const token = m_stack.back();
			if (auto const* paren = std::get_if<Paren>(&token.term);
				paren != nullptr && *paren == Paren::Left) {
				break;
			}
			auto const op_stack = std::get<BinaryOp>(token.term);
			auto const p_stack = bin_op_precedence_v.at(op_stack);
			if (p_current > p_stack) { break; }
			m_output.push_back(cast(token));
			m_stack.pop_back();
		}
		m_stack.push_back(m_current);
	}

	Token m_current{};
	std::vector<Token> m_stack{};
	std::vector<RpnToken> m_output{};
};
} // namespace shunt
