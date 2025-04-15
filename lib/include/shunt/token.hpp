#pragma once
#include <shunt/loc.hpp>
#include <shunt/types.hpp>
#include <variant>

namespace shunt {
struct Token {
	using Type = std::variant<Paren, BinaryOp, Call, Operand>;

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
