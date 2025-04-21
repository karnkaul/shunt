#pragma once
#include <shunt/call_table.hpp>
#include <shunt/result.hpp>
#include <shunt/token.hpp>
#include <vector>

namespace shunt {
struct Infix {
	std::vector<Token> tokens{};
};

struct Postfix {
	std::vector<Token> tokens{};
};

class Interpreter {
  public:
	explicit Interpreter(CallTable const* call_table = nullptr) { set_call_table(call_table); }

	void set_call_table(CallTable const* call_table);

	[[nodiscard]] auto tokenize_to_infix(std::string_view line) -> Result<Infix>;

	[[nodiscard]] auto parse_to_postfix(std::string_view line) -> Result<Postfix>;
	[[nodiscard]] auto parse_to_postfix(Infix const& infix) -> Result<Postfix>;

	[[nodiscard]] auto evaluate(std::string_view line) -> Result<double>;
	[[nodiscard]] auto evaluate(Infix const& infix) -> Result<double>;
	[[nodiscard]] auto evaluate(Postfix const& postfix) -> Result<double>;

  private:
	CallTable const* m_call_table{};

	std::vector<Token> m_postfix{};
	std::vector<Token> m_stack{};
	std::vector<Operand> m_operands{};
};
} // namespace shunt
