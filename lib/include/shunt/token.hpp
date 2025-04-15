#pragma once
#include <shunt/loc.hpp>
#include <shunt/types.hpp>
#include <variant>

namespace shunt {
struct Token {
	using Type = std::variant<Paren, BinaryOp, Call, Operand>;

	Type type{};
	std::string_view lexeme{};
	Loc loc{};
};

[[nodiscard]] auto to_string(Token::Type const& token_type) -> std::string;
} // namespace shunt
