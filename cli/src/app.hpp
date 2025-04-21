#pragma once
#include <shunt/runner/runner.hpp>
#include <span>

namespace shunt::cli {
class App {
  public:
	auto run(int argc, char const* const* argv) -> int;

  private:
	auto evaluate(std::span<std::string_view const> args) -> bool;

	void interactive_session();
	auto try_builtin() -> bool;

	Runner m_runner{};

	std::string m_line{};
};
} // namespace shunt::cli
