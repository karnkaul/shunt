#include <shunt/binop.hpp>
#include <cmath>

namespace shunt {
auto Binop::evaluate(double const a, double const b) const -> double {
	switch (type()) {
	case Type::Plus: return a + b;
	case Type::Minus: return a - b;
	case Type::Star: return a * b;
	case Type::Slash: return a / b;
	case Type::Caret: return std::pow(a, b);
	default: return 0.0;
	}
}
} // namespace shunt
