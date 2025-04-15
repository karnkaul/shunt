#pragma once
#include <klib/enum_array.hpp>
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

inline constexpr auto bin_ops_v = [] {
	auto ret = std::array<BinaryOp, std::size_t(BinaryOp::COUNT_)>{};
	for (std::size_t i = 0; i < ret.size(); ++i) { ret.at(i) = BinaryOp(i); }
	return ret;
}();

inline constexpr auto bin_op_symbol_v = klib::EnumArray<BinaryOp, std::string_view>{
	"+", "-", "*", "/", "^",
};

inline constexpr auto bin_op_precedence_v = klib::EnumArray<BinaryOp, int>{
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
