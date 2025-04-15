#pragma once
#include <shunt/result.hpp>
#include <shunt/token.hpp>
#include <span>
#include <vector>

namespace shunt {
[[nodiscard]] auto scan_tokens(std::string_view line) -> Result<std::vector<Token>>;

[[nodiscard]] auto parse_to_rpn(std::span<Token const> tokens) -> Result<std::vector<RpnToken>>;

[[nodiscard]] auto evaluate(std::span<RpnToken const> rpn_stack) -> Result<double>;
} // namespace shunt
