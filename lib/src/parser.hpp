#pragma once
#include <klib/visitor.hpp>
#include <shunt/result.hpp>
#include <shunt/token.hpp>
#include <cassert>
#include <span>
#include <vector>

namespace shunt {
class Parser {
  public:
	auto parse(std::span<Token const> tokens) -> Result<std::vector<Token>> {
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

				if (std::holds_alternative<Paren>(token.type)) {
					throw SyntaxError{
						.description = "Mismatched opening parenthesis",
						.lexeme = token.lexeme,
						.loc = token.loc,
					};
				}
				if (std::holds_alternative<BinaryOp>(token.type) ||
					std::holds_alternative<Call>(token.type)) {
					m_output.push_back(token);
				} else {
					throw SyntaxError{.description = "ICE"};
				}
			}
		} catch (SyntaxError const& error) { return std::unexpected(error); }
		return std::move(m_output);
	}

  private:
	void parse_current() {
		auto const visitor = klib::Visitor{
			[this](Paren const paren) {
				switch (paren) {
				case Paren::Left: m_stack.push_back(m_current); break;
				case Paren::Right: on_paren_r(); break;
				}
			},
			[this](BinaryOp const /*op*/) { apply_bin_op(); },
			[this](Call const /*func*/) { m_stack.push_back(m_current); },
			[this](Operand const /*op*/) { m_output.push_back(m_current); },
		};
		std::visit(visitor, m_current.type);
	}

	void on_paren_r() {
		while (!m_stack.empty()) {
			auto const token = m_stack.back();
			m_stack.pop_back();
			if (auto const* paren = std::get_if<Paren>(&token.type);
				paren != nullptr && *paren == Paren::Left) {
				pop_if_func();
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

	void pop_if_func() {
		if (m_stack.empty()) { return; }
		auto const token = m_stack.back();
		if (!std::holds_alternative<Call>(token.type)) { return; }
		m_stack.pop_back();
		m_output.push_back(token);
	}

	void apply_bin_op() {
		auto const op = std::get<BinaryOp>(m_current.type);
		auto const p_current = bin_op_precedence_v.at(op);
		while (!m_stack.empty()) {
			auto const token = m_stack.back();
			if (auto const* paren = std::get_if<Paren>(&token.type);
				paren != nullptr && *paren == Paren::Left) {
				break;
			}
			auto const op_stack = std::get<BinaryOp>(token.type);
			auto const p_stack = bin_op_precedence_v.at(op_stack);
			if (p_current > p_stack) { break; }
			m_output.push_back(token);
			m_stack.pop_back();
		}
		m_stack.push_back(m_current);
	}

	Token m_current{};
	std::vector<Token> m_stack{};
	std::vector<Token> m_output{};
};
} // namespace shunt
