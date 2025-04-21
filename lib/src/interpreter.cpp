#include <detail/eval_sink.hpp>
#include <detail/parser.hpp>
#include <detail/rpn_sink.hpp>
#include <detail/tokenizer.hpp>
#include <shunt/interpreter.hpp>
#include <cassert>

namespace shunt {
namespace {
auto const g_call_table = default_call_table();

template <typename F>
[[nodiscard]] auto tokenize(std::string_view const line, F per_token) -> Result<void> {
	auto tokenizer = detail::Tokenizer{line};
	while (true) {
		auto const token = tokenizer.scan_next();
		if (!token) { return std::unexpected(token.error()); }
		auto const result = per_token(*token);
		if (!result) { return std::unexpected(result.error()); }
		if (token->is<Eof>()) { return {}; }
	}
}
} // namespace

void Interpreter::set_call_table(CallTable const* call_table) {
	m_call_table = call_table != nullptr ? call_table : &g_call_table;
}

auto Interpreter::scan_to_infix(std::string_view const line) -> Result<Infix> {
	if (line.empty()) { return {}; }
	m_output.clear();
	auto const per_token = [this](Token const& token) {
		m_output.push_back(token);
		return Result<void>{};
	};
	auto const result = tokenize(line, per_token);
	if (!result) { return std::unexpected(result.error()); }
	return Infix{.tokens = std::move(m_output)};
}

auto Interpreter::scan_to_postfix(std::string_view const line) -> Result<Postfix> {
	if (line.empty()) { return {}; }
	auto sink = detail::RpnSink{m_output};
	auto parser = detail::Parser{m_stack, sink};
	auto const per_token = [&](Token const& token) -> Result<void> {
		auto const result = parser.parse_next(token);
		if (!result) { return std::unexpected(result.error()); }
		return {};
	};
	auto const result = tokenize(line, per_token);
	if (!result) { return std::unexpected(result.error()); }
	return Postfix{.tokens = std::move(m_output)};
}

auto Interpreter::parse_to_postfix(Infix const& infix) -> Result<Postfix> {
	if (infix.tokens.empty()) { return {}; }
	if (!infix.tokens.back().is<Eof>()) {
		return std::unexpected(SyntaxError{.description = "Invalid expression"});
	}
	auto sink = detail::RpnSink{m_output};
	auto parser = detail::Parser{m_stack, sink};
	for (auto const token : infix.tokens) {
		auto const result = parser.parse_next(token);
		if (!result) { return std::unexpected(result.error()); }
	}
	return Postfix{.tokens = std::move(m_output)};
}

auto Interpreter::evaluate(Postfix const& postfix) -> Result<double> {
	if (auto const result = pre_evaluate(postfix.tokens)) { return *result; }

	auto sink = detail::EvalSink{*m_call_table, m_operands};
	for (auto const& token : postfix.tokens) {
		if (token.is<Eof>()) {
			sink.on_eof(token);
		} else if (token.is<Operator>()) {
			sink.on_operator(token);
		} else if (token.is<Operand>()) {
			sink.on_operand(token);
		} else if (token.is<Call>()) {
			sink.on_call(token);
		} else {
			return std::unexpected(SyntaxError{.description = "Invalid expression"});
		}
	}

	return top_operand();
}

auto Interpreter::evaluate(Infix const& infix) -> Result<double> {
	if (auto const result = pre_evaluate(infix.tokens)) { return *result; }

	auto sink = detail::EvalSink{*m_call_table, m_operands};
	auto parser = detail::Parser{m_stack, sink};
	for (auto const& token : infix.tokens) {
		auto const result = parser.parse_next(token);
		if (!result) { return std::unexpected(result.error()); }
	}

	return top_operand();
}

auto Interpreter::evaluate(std::string_view const line) -> Result<double> {
	assert(m_call_table);
	if (line.empty()) { return {}; }

	auto tokenizer = detail::Tokenizer{line};
	auto sink = detail::EvalSink{*m_call_table, m_operands};
	auto parser = detail::Parser{m_stack, sink};

	auto const per_token = [&](Token const& token) -> Result<void> {
		auto const result = parser.parse_next(token);
		if (!result) { return std::unexpected(result.error()); }
		return {};
	};
	auto const result = tokenize(line, per_token);
	if (!result) { return std::unexpected(result.error()); }

	return top_operand();
}

auto Interpreter::pre_evaluate(std::span<Token const> tokens) const
	-> std::optional<Result<double>> {
	assert(m_call_table);
	if (tokens.empty()) { return Result<double>{}; }
	if (!tokens.back().is<Eof>()) {
		return std::unexpected(SyntaxError{.description = "Invalid expression"});
	}
	return std::nullopt;
}

auto Interpreter::top_operand() const -> Result<double> {
	if (m_operands.size() != 1) {
		return std::unexpected(SyntaxError{.description = "Invalid expression"});
	}
	return m_operands.front();
}
} // namespace shunt
