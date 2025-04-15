#include <shunt/syntax_error.hpp>
#include <format>

namespace shunt {
auto SyntaxError::format(std::string_view const input) const -> std::string {
	auto ret = std::string{};
	if (description.empty()) {
		ret.append("Unknown error");
	} else {
		ret.append(description);
	}
	if (lexeme.empty()) { return ret; }
	std::format_to(std::back_inserter(ret), ": '{}' (col {})", lexeme, loc.start + 1);
	if (input.empty() || loc.length == 0) { return ret; }
	std::format_to(std::back_inserter(ret), "\n  {}\n", input);
	ret.resize(ret.size() + 2 + loc.start, ' ');
	ret.resize(ret.size() + loc.length, '^');
	return ret;
}
} // namespace shunt
