#pragma once
#include <shunt/interpreter.hpp>
#include <shunt/runner/printer.hpp>

namespace shunt {
[[nodiscard]] auto default_printer() -> IPrinter const&;

struct RunnerArgs {
	CallTable const* call_table{nullptr};
	IPrinter const* printer{&default_printer()};
	bool verbose{false};
};

class Runner {
  public:
	using Args = RunnerArgs;

	explicit Runner(Args const& args = {});

	auto evaluate(std::string_view line) -> bool;

	bool verbose{};

  private:
	template <typename T>
	[[nodiscard]] auto is_error(Result<T> const& result, std::string_view line) const -> bool;

	[[nodiscard]] auto eval_verbose(std::string_view line) -> bool;

	IPrinter const* m_printer{};

	Interpreter m_interpreter{};
};
} // namespace shunt
