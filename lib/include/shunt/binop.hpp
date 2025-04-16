#pragma once
#include <klib/enum_array.hpp>
#include <cassert>
#include <cstdint>
#include <string_view>

namespace shunt {
enum class BinopType : std::int8_t {
	Plus,
	Minus,
	Star,
	Slash,
	Caret,
	COUNT_,
};

class Binop {
  public:
	using Type = BinopType;

	static constexpr auto count_v = std::size_t(Type::COUNT_);

	explicit(false) constexpr Binop(Type type) : m_type(type) {
		assert(m_type >= Type{} && m_type < Type::COUNT_);
	}

	[[nodiscard]] constexpr auto type() const -> Type { return m_type; }
	[[nodiscard]] constexpr auto symbol() const -> std::string_view { return symbol_v.at(type()); }
	[[nodiscard]] constexpr auto precedence() const -> int { return precedence_v.at(type()); }

	[[nodiscard]] auto evaluate(double a, double b) const -> double;

	auto operator==(Binop const&) const -> bool = default;

	constexpr operator Type() const { return type(); }

  private:
	static constexpr auto types_v = [] {
		auto ret = std::array<Type, std::size_t(Type::COUNT_)>{};
		for (std::size_t i = 0; i < ret.size(); ++i) { ret.at(i) = Type(i); }
		return ret;
	}();

	static constexpr auto symbol_v = klib::EnumArray<Type, std::string_view>{
		"+", "-", "*", "/", "^",
	};

	static constexpr auto precedence_v = klib::EnumArray<Type, int>{
		1, 1, 2, 2, 3,
	};

	Type m_type{};
};

namespace detail {
template <std::size_t... I>
constexpr auto make_binops(std::index_sequence<I...> /*s*/) {
	return std::array<Binop, sizeof...(I)>{Binop{Binop::Type(I)}...};
}
} // namespace detail

inline constexpr auto binops_v = detail::make_binops(std::make_index_sequence<Binop::count_v>());
} // namespace shunt
