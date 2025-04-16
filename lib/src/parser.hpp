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
	explicit Parser(std::vector<Token>& out, std::vector<Token>& stack)
		: m_stack(stack), m_output(out) {}

	auto parse(std::span<Token const> tokens) -> Result<void> {
		m_stack.clear();
		m_output.clear();
		m_current = {};

		try {
			for (auto const token : tokens) {
				m_current = token;
				parse_current();
			}
			drain_stack();
		} catch (SyntaxError const& error) { return std::unexpected(error); }
		return {};
	}

  private:
	void parse_current() {
		auto const visitor = klib::Visitor{
			[this](Paren const paren) {
				switch (paren) {
				case Paren::Left: m_stack.push_back(m_current); break;
				case Paren::Right: on_paren_r(); break;
				default: throw SyntaxError{.description = "ICE"};
				}
			},
			[this](Binop) { apply_bin_op(); },
			[this](Call) { m_stack.push_back(m_current); },
			[this](Operand) { m_output.push_back(m_current); },
		};
		std::visit(visitor, m_current.type);
	}

	void drain_stack() {
		while (!m_stack.empty()) {
			auto const token = m_stack.back();
			m_stack.pop_back();

			if (token.is<Binop>() || token.is<Call>()) {
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
			if (auto const* paren = token.get_if<Paren>();
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
		if (!token.is<Call>()) { return; }
		m_stack.pop_back();
		m_output.push_back(token);
	}

	void apply_bin_op() {
		auto const p_current = m_current.get<Binop>().precedence();
		while (!m_stack.empty()) {
			auto const token = m_stack.back();
			if (auto const* paren = token.get_if<Paren>();
				paren != nullptr && *paren == Paren::Left) {
				break;
			}
			auto const p_stack = token.get<Binop>().precedence();
			if (p_current > p_stack) { break; }
			m_output.push_back(token);
			m_stack.pop_back();
		}
		m_stack.push_back(m_current);
	}

	Token m_current{};
	std::vector<Token>& m_stack;
	std::vector<Token>& m_output;
};
} // namespace shunt
