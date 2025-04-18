#include <shunt/shunt.hpp>
#include <print>

auto main() -> int {
	try {
		static constexpr auto line = std::string_view{"10 - 4 + 20 * 3"};
		auto const tokens = shunt::scan_tokens(line);
		if (!tokens) {
			std::println(stderr, "{}", tokens.error().format(line));
			return EXIT_FAILURE;
		}

		auto const rpn = shunt::parse_to_rpn(tokens.value());
		if (!rpn) {
			std::println(stderr, "{}", rpn.error().format(line));
			return EXIT_FAILURE;
		}
		for (auto const& token : rpn.value()) { std::print("{} ", to_string(token.type)); }
		std::println();

		auto const value = shunt::evaluate(rpn.value());
		if (!value) {
			std::println(stderr, "{}", value.error().format(line));
			return EXIT_FAILURE;
		}
		std::println("{} = {}", line, value.value());
	} catch (std::exception const& e) {
		std::println("PANIC: {}", e.what());
		return EXIT_FAILURE;
	}
}
