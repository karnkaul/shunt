#include <detail/tokenizer.hpp>
#include <evaluator.hpp>
#include <parser.hpp>
#include <shunt/interpreter.hpp>
#include <cassert>

namespace shunt {
namespace {
auto const g_call_table = default_call_table();
} // namespace

void Interpreter::set_call_table(CallTable const* call_table) {
	m_call_table = call_table != nullptr ? call_table : &g_call_table;
}

auto Interpreter::scan_to_infix(std::string_view const line) -> Result<Infix> {
	m_output.clear();
	auto tokenizer = detail::Tokenizer{line};
	while (true) {
		auto const token = tokenizer.scan_next();
		if (!token) { return std::unexpected(token.error()); }
		m_output.push_back(*token);
		if (token->is<Eof>()) { return Infix{.tokens = std::move(m_output)}; }
	}
}

auto Interpreter::scan_to_postfix(std::string_view const line) -> Result<Postfix> {
	m_output.clear();
	auto tokenizer = detail::Tokenizer{line};
	auto parser = Parser{m_output, m_stack};
	while (true) {
		auto const token = tokenizer.scan_next();
		if (!token) { return std::unexpected(token.error()); }
		auto const result = parser.parse_next(*token);
		if (!result) { return std::unexpected(result.error()); }
		if (token->is<Eof>()) { break; }
	}
	return Postfix{.tokens = std::move(m_output)};
}

auto Interpreter::parse_to_postfix(Infix const& infix) -> Result<Postfix> {
	if (infix.tokens.empty()) { return {}; }
	if (!infix.tokens.back().is<Eof>()) {
		return std::unexpected(SyntaxError{.description = "Invalid expression"});
	}

	auto parser = Parser{m_output, m_stack};
	for (auto const token : infix.tokens) {
		auto const result = parser.parse_next(token);
		if (!result) { return std::unexpected(result.error()); }
	}
	return Postfix{.tokens = std::move(m_output)};
}

auto Interpreter::evaluate(Postfix const& postfix) -> Result<double> {
	assert(m_call_table);
	auto evaluator = Evaluator{*m_call_table, m_operands};
	return evaluator.evaluate(postfix.tokens);
}

auto Interpreter::evaluate(Infix const& infix) -> Result<double> {
	auto const result = parse_to_postfix(infix);
	if (!result) { return std::unexpected(result.error()); }
	return evaluate(*result);
}

auto Interpreter::evaluate(std::string_view const line) -> Result<double> {
	auto const result = scan_to_postfix(line);
	if (!result) { return std::unexpected(result.error()); }
	return evaluate(*result);
}
} // namespace shunt
