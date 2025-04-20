#pragma once
#include <shunt/interpreter.hpp>
#include <span>

namespace shunt::cli {
class App {
  public:
	auto run(int argc, char const* const* argv) -> int;

  private:
	auto evaluate(std::span<std::string_view const> args) -> bool;
	auto evaluate_line() -> bool;
	auto eval_verbose() -> bool;

	void print_error(SyntaxError const& error) const;

	void interactive_session();
	auto try_builtin() -> bool;

	Interpreter m_interpreter{};

	std::string m_line{};
	bool m_verbose{};
};
} // namespace shunt::cli
