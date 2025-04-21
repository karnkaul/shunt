#include <klib/unit_test.hpp>
#include <nearly_eq.hpp>
#include <shunt/interpreter.hpp>

namespace {
using namespace shunt;

TEST(interpreter_tokenize) {
	auto interpreter = Interpreter{};
	auto const result = interpreter.tokenize_to_infix("(1+2)*3");
	ASSERT(result);
	auto const& tokens = result->tokens;
	ASSERT(tokens.size() == 8);
	EXPECT(tokens.back().is<Eof>());
	ASSERT(tokens[0].is<Paren>());
	EXPECT(tokens[0].get<Paren>() == Paren::Left);
	ASSERT(tokens[1].is<Operand>());
	EXPECT(nearly_eq(tokens[1].get<Operand>(), 1.0));
	ASSERT(tokens[2].is<Operator>());
	EXPECT(tokens[2].get<Operator>().type() == Operator::Type::Plus);
	ASSERT(tokens[3].is<Operand>());
	EXPECT(nearly_eq(tokens[3].get<Operand>(), 2.0));
	ASSERT(tokens[4].is<Paren>());
	EXPECT(tokens[4].get<Paren>() == Paren::Right);
	ASSERT(tokens[5].is<Operator>());
	EXPECT(tokens[5].get<Operator>().type() == Operator::Type::Star);
	ASSERT(tokens[6].is<Operand>());
	EXPECT(nearly_eq(tokens[6].get<Operand>(), 3.0));
}

TEST(interpreter_parse_line) {
	auto interpreter = Interpreter{};
	auto const result = interpreter.parse_to_postfix("(1+2)*3");
	auto const& tokens = result->tokens;
	ASSERT(tokens.size() == 6);
	EXPECT(tokens.back().is<Eof>());
	ASSERT(tokens[0].is<Operand>());
	EXPECT(nearly_eq(tokens[0].get<Operand>(), 1.0));
	ASSERT(tokens[1].is<Operand>());
	EXPECT(nearly_eq(tokens[1].get<Operand>(), 2.0));
	ASSERT(tokens[2].is<Operator>());
	EXPECT(tokens[2].get<Operator>().type() == Operator::Type::Plus);
	ASSERT(tokens[3].is<Operand>());
	EXPECT(nearly_eq(tokens[3].get<Operand>(), 3.0));
	ASSERT(tokens[4].is<Operator>());
	EXPECT(tokens[4].get<Operator>().type() == Operator::Type::Star);
}

TEST(interpreter_eval_line) {
	auto interpreter = Interpreter{};
	auto result = interpreter.evaluate("(1+2)*3");
	ASSERT(result);
	EXPECT(nearly_eq(*result, 9.0));

	result = interpreter.evaluate("abs(-8) / 2");
	ASSERT(result);
	EXPECT(nearly_eq(*result, 4.0));
}
} // namespace
