#pragma once
#include <shunt/loc.hpp>
#include <shunt/term.hpp>
#include <string>

namespace shunt {
struct SyntaxError {
	[[nodiscard]] auto format(std::string_view input) const -> std::string;

	std::string_view description{};
	std::string_view lexeme{};
	Loc loc{};
};
} // namespace shunt
