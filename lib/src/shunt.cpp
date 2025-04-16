#include <evaluator.hpp>
#include <parser.hpp>
#include <scanner.hpp>
#include <shunt/shunt.hpp>

auto shunt::scan_tokens(std::string_view const line) -> Result<std::vector<Token>> {
	return Scanner{}.scan(line);
}

auto shunt::parse_to_rpn(std::span<Token const> tokens) -> Result<std::vector<Token>> {
	return Parser{}.parse(tokens);
}

auto shunt::evaluate(std::span<Token const> rpn_stack, CallTable const* call_table)
	-> Result<double> {
	static auto const s_call_table = default_call_table();
	if (call_table == nullptr) { call_table = &s_call_table; }
	return Evaluator{*call_table}.evaluate(rpn_stack);
}
