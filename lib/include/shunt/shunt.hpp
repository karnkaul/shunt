#pragma once
#include <shunt/call_table.hpp>
#include <shunt/result.hpp>
#include <shunt/token.hpp>
#include <span>
#include <vector>

namespace shunt {
[[nodiscard]] auto scan_tokens(std::string_view line) -> Result<std::vector<Token>>;

[[nodiscard]] auto parse_to_rpn(std::span<Token const> tokens) -> Result<std::vector<Token>>;

[[nodiscard]] auto evaluate(std::span<Token const> rpn_stack, CallTable const* call_table = {})
	-> Result<double>;
} // namespace shunt
