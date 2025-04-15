#include <shunt/term.hpp>
#include <shunt/visitor.hpp>
#include <format>

namespace shunt {
namespace {
struct ToString {
	auto operator()(Paren const p) const -> std::string { return p == Paren::Left ? "(" : ")"; }
	auto operator()(BinaryOp const& op) const { return std::string{bin_op_symbol_v.at(op)}; }
	auto operator()(Call const func) const { return std::string{func}; }
	auto operator()(Operand const op) const { return std::format("{}", op); }
};
} // namespace
} // namespace shunt

auto shunt::to_string(Term const& term) -> std::string { return std::visit(ToString{}, term); }

auto shunt::to_string(RpnTerm const& term) -> std::string { return std::visit(ToString{}, term); }
