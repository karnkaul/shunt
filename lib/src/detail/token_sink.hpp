#pragma once
#include <klib/base_types.hpp>
#include <shunt/token.hpp>

namespace shunt::detail {
struct ITokenSink : klib::Polymorphic {
	virtual void on_operator(Token const& token) = 0;
	virtual void on_operand(Token const& token) = 0;
	virtual void on_call(Token const& token) = 0;
	virtual void on_eof(Token const& token) = 0;
};
} // namespace shunt::detail
