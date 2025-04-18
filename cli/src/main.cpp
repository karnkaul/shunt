#include <app.hpp>
#include <shunt/interpreter.hpp>
#include <print>

auto main(int argc, char** argv) -> int {
	try {
		return shunt::cli::App{}.run(argc, argv);
	} catch (std::exception const& e) {
		std::println(stderr, "PANIC: {}", e.what());
		return EXIT_FAILURE;
	} catch (...) {
		std::println(stderr, "PANIC!");
		return EXIT_FAILURE;
	}
}
