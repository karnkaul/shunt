#pragma once
#include <klib/base_types.hpp>
#include <string_view>

namespace shunt {
struct IPrinter : klib::Polymorphic {
	virtual void print(std::string_view text) const = 0;
	virtual void print_error(std::string_view text) const = 0;
};
} // namespace shunt
