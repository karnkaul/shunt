#pragma once
#include <shunt/enum_array.hpp>
#include <cstdint>
#include <string>
#include <variant>

namespace shunt {
using Operand = double;

enum class BinaryOp : std::int8_t {
	Plus,
	Minus,
	Star,
	Slash,
	Caret,
	COUNT_,
};

inline constexpr auto bin_op_symbol_v = EnumArray<BinaryOp, std::string_view>{
	"+", "-", "*", "/", "^",
};

inline constexpr auto bin_op_precedence_v = EnumArray<BinaryOp, int>{
	1, 1, 2, 2, 3,
};

using Call = std::string_view;

enum class Paren : std::int8_t {
	Left,
	Right,
};

using Term = std::variant<Paren, BinaryOp, Call, Operand>;
using RpnTerm = std::variant<BinaryOp, Call, Operand>;

[[nodiscard]] auto to_string(Term const& term) -> std::string;
[[nodiscard]] auto to_string(RpnTerm const& term) -> std::string;
} // namespace shunt
