#pragma once
#include <shunt/call.hpp>
#include <shunt/loc.hpp>
#include <shunt/operator.hpp>
#include <variant>

namespace shunt {
struct Eof {};

using Operand = double;

enum class Paren : std::int8_t {
	Left,
	Right,
};

struct Token {
	using Type = std::variant<Eof, Paren, Operator, Call, Operand>;

	template <typename T>
	[[nodiscard]] constexpr auto is() const -> bool {
		return std::holds_alternative<T>(type);
	}

	template <typename T>
	[[nodiscard]] constexpr auto get_if() const -> T const* {
		return std::get_if<T>(&type);
	}

	template <typename T>
	[[nodiscard]] constexpr auto get() const -> T const& {
		return std::get<T>(type);
	}

	Type type{};
	std::string_view lexeme{};
	Loc loc{};
};

[[nodiscard]] auto to_string(Token::Type const& token_type) -> std::string;
} // namespace shunt
