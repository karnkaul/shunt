#include <detail/tokenizer.hpp>
#include <evaluator.hpp>
#include <parser.hpp>
#include <shunt/shunt.hpp>

auto shunt::scan_tokens(std::string_view const line) -> Result<std::vector<Token>> {
	auto ret = std::vector<Token>{};
	auto tokenizer = detail::Tokenizer{line};
	while (true) {
		auto const token = tokenizer.scan_next();
		if (!token) { return std::unexpected(token.error()); }
		ret.push_back(*token);
		if (token->is<Eof>()) { return ret; }
	}
}

auto shunt::parse_to_rpn(std::span<Token const> tokens) -> Result<std::vector<Token>> {
	auto ret = std::vector<Token>{};
	auto stack = std::vector<Token>{};
	auto const result = Parser{ret, stack}.parse(tokens);
	if (!result) { return std::unexpected(result.error()); }
	return ret;
}

auto shunt::evaluate(std::span<Token const> rpn_stack, CallTable const* call_table)
	-> Result<double> {
	static auto const s_call_table = default_call_table();
	if (call_table == nullptr) { call_table = &s_call_table; }
	auto operands = std::vector<Operand>{};
	return Evaluator{*call_table, operands}.evaluate(rpn_stack);
}
