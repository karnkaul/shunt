#pragma once
#include <shunt/syntax_error.hpp>
#include <expected>

namespace shunt {
template <typename ValueT>
using Result = std::expected<ValueT, SyntaxError>;
} // namespace shunt
