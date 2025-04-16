#include <shunt/call_table.hpp>
#include <cmath>

auto shunt::default_call_table() -> CallTable {
	auto ret = std::unordered_map<Call, Func>{};
	ret["abs"] = [](double const in) { return std::abs(in); };
	return ret;
}

auto shunt::find_func(CallTable const& table, Call const call) -> Func {
	auto const it = table.find(call);
	if (it == table.end()) { return {}; }
	return it->second;
}
