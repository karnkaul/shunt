#pragma once
#include <shunt/result.hpp>
#include <shunt/token.hpp>
#include <cassert>
#include <charconv>
#include <vector>

namespace shunt {
class Scanner {
  public:
	explicit Scanner(std::vector<Token>& out) : m_out(out) {}

	[[nodiscard]] auto scan(std::string_view line) -> Result<void> {
		m_out.clear();
		m_remain = line;
		m_index = 0;

		try {
			for (auto token = Token{}; next(token);) { m_out.push_back(token); }
		} catch (SyntaxError const& error) { return std::unexpected(error); }
		m_remain = {};
		return {};
	}

  private:
	[[nodiscard]] static constexpr auto is_space(char const c) -> bool {
		return c == ' ' || c == '\t' || c == '\n' || c == '\r';
	}

	[[nodiscard]] static constexpr auto is_digit(char const c) -> bool {
		return c >= '0' && c <= '9';
	}

	[[nodiscard]] static constexpr auto is_alpha(char const c) -> bool {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}

	[[nodiscard]] static constexpr auto is_paren(char const c) -> bool {
		return c == '(' || c == ')';
	}

	[[nodiscard]] auto next(Token& out) -> bool {
		trim_front();
		if (m_remain.empty()) { return false; }
		out = next_token();
		return true;
	}

	[[nodiscard]] auto peek() const -> char { return m_remain.empty() ? '\0' : m_remain.front(); }

	[[nodiscard]] auto next_token() -> Token {
		assert(!m_remain.empty());
		char const ch = m_remain.front();
		if (is_paren(ch)) { return scan_paren(); }
		if (is_alpha(ch)) { return scan_function(); }
		if (is_digit(ch)) { return scan_number(); }
		return scan_operator();
	}

	[[nodiscard]] auto scan_paren() -> Token {
		auto const ch = m_remain.front();
		auto const ret = Token{
			.type = (ch == '(') ? Paren::Left : Paren::Right,
			.lexeme = m_remain.substr(0, 1),
			.loc = {.start = m_index, .length = 1},
		};
		m_remain = m_remain.substr(1);
		++m_index;
		return ret;
	}

	[[nodiscard]] auto scan_function() -> Token {
		auto last = std::size_t{};
		for (; last < m_remain.size(); ++last) {
			auto const ch = m_remain.at(last);
			if (!is_alpha(ch) && !is_digit(ch) && ch != '_') { break; }
		}
		auto const func_name = m_remain.substr(0, last);
		m_remain = m_remain.substr(last);
		auto const start = m_index;
		m_index += last;
		return Token{
			.type = func_name,
			.lexeme = func_name,
			.loc = {.start = start, .length = last},
		};
	}

	[[nodiscard]] auto scan_number() -> Token {
		auto value = double{};
		auto const [ptr, ec] =
			std::from_chars(m_remain.data(), m_remain.data() + m_remain.size(), value);
		if (ec != std::errc{}) {
			throw SyntaxError{
				.description = "Invalid number",
				.lexeme = m_remain.substr(0, 1),
				.loc = {.start = m_index, .length = 1},
			};
		}
		auto const distance = std::size_t(std::distance(m_remain.data(), ptr));
		auto ret = Token{.type = value, .lexeme = m_remain.substr(0, distance)};
		ret.loc = {.start = m_index, .length = distance};
		m_remain = m_remain.substr(distance);
		m_index += distance;
		return ret;
	}

	[[nodiscard]] auto scan_operator() -> Token {
		for (auto const op : operators_v) {
			auto const symbol = op.symbol();
			if (m_remain.starts_with(symbol)) {
				auto ret = Token{.type = op, .lexeme = m_remain.substr(0, symbol.size())};
				ret.loc = {.start = m_index, .length = symbol.size()};
				m_remain = m_remain.substr(symbol.size());
				m_index += symbol.size();
				return ret;
			}
		}

		throw SyntaxError{
			.description = "Unrecognized operator",
			.lexeme = m_remain.substr(0, 1),
			.loc = {.start = m_index, .length = 1},
		};
	}

	void trim_front() {
		while (!m_remain.empty() && is_space(m_remain.front())) {
			m_remain = m_remain.substr(1);
			++m_index;
		}
	}

	std::vector<Token>& m_out;

	std::string_view m_remain{};
	std::size_t m_index{};
};
} // namespace shunt
