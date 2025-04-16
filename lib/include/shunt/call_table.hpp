#pragma once
#include <shunt/call.hpp>
#include <unordered_map>

namespace shunt {
using Func = double (*)(double in);
using CallTable = std::unordered_map<Call, Func>;

[[nodiscard]] auto default_call_table() -> CallTable;
[[nodiscard]] auto find_func(CallTable const& table, Call call) -> Func;
} // namespace shunt
