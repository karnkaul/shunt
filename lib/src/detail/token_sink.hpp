#pragma once
#include <klib/base_types.hpp>
#include <shunt/token.hpp>

namespace shunt::detail {
struct ITokenSink : klib::Polymorphic {
	virtual void on_output(Token const& token) = 0;
};
} // namespace shunt::detail
