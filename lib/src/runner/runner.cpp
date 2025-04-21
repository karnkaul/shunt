#include <klib/escape_code.hpp>
#include <shunt/runner/runner.hpp>
#include <print>

namespace shunt {
namespace {
struct Printer : IPrinter {
	void print(std::string_view const text) const final { std::print("{}", text); }

	void print_error(std::string_view const text) const final {
		static auto const s_color = klib::escape::foreground({220, 15, 15});
		std::println(stderr, "{}{}{}", s_color, text, klib::escape::clear);
	}
};

auto const g_printer = Printer{};
} // namespace

Runner::Runner(Args const& args)
	: verbose(args.verbose), m_printer(args.printer != nullptr ? args.printer : &g_printer),
	  m_interpreter(args.call_table) {}

auto Runner::evaluate(std::string_view const line) -> bool {
	if (line.empty()) { return true; }

	if (verbose) { return eval_verbose(line); }

	auto const result = m_interpreter.evaluate(line);
	if (is_error(result, line)) { return false; }

	m_printer->print(std::format("{}\n", *result));
	return true;
}

template <typename T>
auto Runner::is_error(Result<T> const& result, std::string_view const line) const -> bool {
	if (!result) {
		m_printer->print_error(result.error().format(line));
		return true;
	}
	return false;
}

auto Runner::eval_verbose(std::string_view const line) -> bool {
	auto const print_tokens = [this](std::string_view const prefix, std::span<Token const> tokens) {
		m_printer->print(prefix);
		for (auto const& token : tokens) {
			m_printer->print(std::format("{} ", to_string(token.type)));
		}
		m_printer->print("\n");
	};

	auto const infix = m_interpreter.tokenize_to_infix(line);
	if (is_error(infix, line)) { return false; }
	print_tokens("infix: ", infix->tokens);

	auto const postfix = m_interpreter.parse_to_postfix(*infix);
	if (is_error(postfix, line)) { return false; }
	print_tokens("postfix: ", postfix->tokens);

	auto const result = m_interpreter.evaluate(*postfix);
	if (is_error(result, line)) { return false; }
	m_printer->print(std::format("result: {}\n\n", *result));

	return true;
}
} // namespace shunt

auto shunt::default_printer() -> IPrinter const& { return shunt::g_printer; }
