#include <parser.hpp>
#include <runtime.hpp>
#include <scanner.hpp>
#include <shunt/shunt.hpp>

auto shunt::scan_tokens(std::string_view const line) -> Result<std::vector<Token>> {
	return Scanner{}.scan(line);
}

auto shunt::parse_to_rpn(std::span<Token const> tokens) -> Result<std::vector<RpnToken>> {
	return Parser{}.parse(tokens);
}

auto shunt::evaluate(std::span<RpnToken const> rpn_stack) -> Result<double> {
	auto eval = Eval{};
	return Runtime{eval}.evaluate(rpn_stack);
}
