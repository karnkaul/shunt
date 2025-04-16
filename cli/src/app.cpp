#include <app.hpp>
#include <klib/args/parse.hpp>
#include <array>
#include <iostream>
#include <print>

namespace shunt::cli {
auto App::run(int const argc, char const* const* argv) -> int {
	auto expr = std::vector<std::string_view>{};
	auto const args = std::array{
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

	auto const result = m_interpreter.evaluate(m_line);
	if (!result) {
		print_error(result.error());
		return false;
	}

	std::println("{}", *result);
	return true;
}

void App::print_error(SyntaxError const& error) const {
	std::println(stderr, "{}", error.format(m_line));
}

void App::interactive_session() {
	std::print("> ");
	while (std::getline(std::cin, m_line)) {
		if (m_line == "quit" || m_line == "exit") { return; }
		evaluate_line();
		std::print("> ");
	}
}
} // namespace shunt::cli
