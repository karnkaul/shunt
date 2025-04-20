#include <app.hpp>
#include <klib/args/parse.hpp>
#include <klib/escape_code.hpp>
#include <array>
#include <iostream>
#include <print>

namespace shunt::cli {
auto App::run(int const argc, char const* const* argv) -> int {
	auto expr = std::vector<std::string_view>{};
	auto const args = std::array{
		klib::args::named_flag(m_verbose, "v,verbose"),
		klib::args::positional_list(expr, "expression"),
	};
	auto const result = klib::args::parse_main(args, argc, argv);
	if (result.early_return()) { return result.get_return_code(); }

	if (!expr.empty()) { return evaluate(expr) ? EXIT_SUCCESS : EXIT_FAILURE; }

	interactive_session();

	return EXIT_SUCCESS;
}

auto App::evaluate(std::span<std::string_view const> args) -> bool {
	if (args.empty()) { return true; }
	m_line.clear();
	for (auto const arg : args) { std::format_to(std::back_inserter(m_line), "{} ", arg); }
	if (!m_line.empty()) { m_line.pop_back(); }
	return evaluate_line();
}

auto App::evaluate_line() -> bool {
	if (m_line.empty()) { return true; }

	if (m_verbose) { return eval_verbose(); }

	auto const result = m_interpreter.evaluate(m_line);
	if (!result) {
		print_error(result.error());
		return false;
	}

	std::println("{}", *result);
	return true;
}

auto App::eval_verbose() -> bool {
	auto const infix = m_interpreter.scan_to_infix(m_line);
	if (!infix) {
		print_error(infix.error());
		return false;
	}

	std::print("infix: ");
	for (auto const& token : infix->tokens) { std::print("{} ", to_string(token.type)); }
	std::println();

	auto const postfix = m_interpreter.parse_to_postfix(*infix);
	if (!postfix) {
		print_error(infix.error());
		return false;
	}

	std::print("postfix: ");
	for (auto const& token : postfix->tokens) { std::print("{} ", to_string(token.type)); }
	std::println();

	auto const result = m_interpreter.evaluate(*postfix);
	if (!result) {
		print_error(result.error());
		return false;
	}

	std::println("result: {}\n", *result);
	return true;
}

void App::print_error(SyntaxError const& error) const {
	static auto const s_color = klib::escape::foreground({220, 15, 15});
	std::println(stderr, "{}{}{}", s_color, error.format(m_line), klib::escape::clear);
}

void App::interactive_session() {
	static auto const s_in_color = klib::escape::foreground({220, 220, 20});
	auto const get_line = [this] {
		std::print("{}> ", s_in_color);
		auto const ret = !!std::getline(std::cin, m_line);
		std::print("{}", klib::escape::clear);
		return ret;
	};

	while (get_line()) {
		if (m_line == "quit" || m_line == "exit") { return; }
		if (!try_builtin()) { evaluate_line(); }
	}
}

auto App::try_builtin() -> bool {
	if (m_line == "verbose") {
		m_verbose = !m_verbose;
		std::println("-- verbose mode turned {}", m_verbose ? "on" : "off");
		return true;
	}

	return false;
}
} // namespace shunt::cli
