#pragma once
#include <array>
#include <type_traits>
#include <utility>

namespace shunt {
template <typename Type>
concept EnumT = std::is_enum_v<Type>;

template <EnumT E, typename Type, std::size_t Size = std::size_t(E::COUNT_)>
struct EnumArray : std::array<Type, Size> {
	using Base = std::array<Type, Size>;

	EnumArray() = default;

	template <std::convertible_to<Type>... Ts>
		requires(sizeof...(Ts) == Size)
	consteval EnumArray(Ts... ts) : Base{std::move(ts)...} {}

	template <typename Self>
	constexpr auto at(this Self&& self, E const e) -> decltype(auto) {
		return std::forward<Self>(self).Base::at(std::size_t(e));
	}
};

template <EnumT E>
constexpr auto increment(E const e) -> E {
	return E(std::to_underlying(e) + 1);
}

template <EnumT E, std::size_t Size = std::size_t(E::COUNT_)>
constexpr auto enumerate() -> std::array<E, Size> {
	auto ret = std::array<E, Size>{};
	for (std::size_t i = 0; i < Size; ++i) { ret.at(i) = E(i); }
	return ret;
}
} // namespace shunt
