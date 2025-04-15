#include <klib/visitor.hpp>
#include <shunt/token.hpp>
#include <format>

auto shunt::to_string(Token::Type const& token_type) -> std::string {
	auto const visitor = klib::Visitor{
		[](Paren const p) -> std::string { return p == Paren::Left ? "(" : ")"; },
		[](BinaryOp const& op) { return std::string{bin_op_symbol_v.at(op)}; },
		[](Call const func) { return std::string{func}; },
		[](Operand const op) { return std::format("{}", op); },
	};
	return std::visit(visitor, token_type);
}
