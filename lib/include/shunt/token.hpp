#pragma once
#include <shunt/loc.hpp>
#include <shunt/term.hpp>

namespace shunt {
template <typename TermT>
struct BasicToken {
	TermT term{};
	std::string_view lexeme{};
	Loc loc{};
};

using Token = BasicToken<Term>;
using RpnToken = BasicToken<RpnTerm>;
} // namespace shunt
