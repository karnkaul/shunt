#pragma once
#include <klib/compat.hpp>

template <std::floating_point Type>
constexpr auto nearly_eq(Type const a, Type const b, Type const epsilon = Type(0.01)) -> bool {
	return klib::abs(a - b) < epsilon;
}
