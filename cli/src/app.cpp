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
		klib::args::named_flag(m_runner.verbose, "v,verbose"),
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
	return m_runner.evaluate(m_line);
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
		if (!try_builtin()) { m_runner.evaluate(m_line); }
	}
}

auto App::try_builtin() -> bool {
	if (m_line == "verbose") {
		m_runner.verbose = !m_runner.verbose;
		std::println("-- verbose mode turned {}", m_runner.verbose ? "on" : "off");
		return true;
	}

	return false;
}
} // namespace shunt::cli
