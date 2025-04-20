#include <detail/tokenizer.hpp>
#include <klib/unit_test.hpp>
#include <nearly_eq.hpp>
#include <format>

namespace {
using namespace shunt;

TEST(tokenizer_empty) {
	auto tokenizer = detail::Tokenizer{""};
	auto token = tokenizer.scan_next();
	ASSERT(token);
	EXPECT(token->is<Eof>());

	tokenizer = detail::Tokenizer{" \n\t "};
	token = tokenizer.scan_next();
	ASSERT(token);
	EXPECT(token->is<Eof>());
}

TEST(tokenizer_paren) {
	auto tokenizer = detail::Tokenizer{"()"};
	auto token = tokenizer.scan_next();
	ASSERT(token);
	ASSERT(token->is<Paren>());
	EXPECT(token->get<Paren>() == Paren::Left);

	token = tokenizer.scan_next();
	ASSERT(token);
	ASSERT(token->is<Paren>());
	EXPECT(token->get<Paren>() == Paren::Right);

	token = tokenizer.scan_next();
	ASSERT(token);
	EXPECT(token->is<Eof>());
}

TEST(tokenizer_operand) {
	static constexpr auto num_v = 1.23456;

	auto num_str = std::format("{}", num_v);
	auto tokenizer = detail::Tokenizer{num_str};
	auto token = tokenizer.scan_next();
	ASSERT(token);
	ASSERT(token->is<Operand>());
	EXPECT(nearly_eq(token->get<Operand>(), num_v));

	token = tokenizer.scan_next();
	ASSERT(token && token->is<Eof>());
}

TEST(tokenizer_call) {
	auto tokenizer = detail::Tokenizer{"foo bar"};
	auto token = tokenizer.scan_next();
	ASSERT(token && token->is<Call>());
	EXPECT(token->get<Call>() == "foo");

	token = tokenizer.scan_next();
	ASSERT(token && token->is<Call>());
	EXPECT(token->get<Call>() == "bar");

	token = tokenizer.scan_next();
	ASSERT(token && token->is<Eof>());
}

TEST(tokenizer_operator) {
	auto tokenizer = detail::Tokenizer{"+- *\t/\n^"};
	auto token = tokenizer.scan_next();
	ASSERT(token && token->is<Operator>());
	EXPECT(token->get<Operator>().type() == OperatorType::Plus);

	token = tokenizer.scan_next();
	ASSERT(token && token->is<Operator>());
	EXPECT(token->get<Operator>().type() == OperatorType::Minus);

	token = tokenizer.scan_next();
	ASSERT(token && token->is<Operator>());
	EXPECT(token->get<Operator>().type() == OperatorType::Star);

	token = tokenizer.scan_next();
	ASSERT(token && token->is<Operator>());
	EXPECT(token->get<Operator>().type() == OperatorType::Slash);

	token = tokenizer.scan_next();
	ASSERT(token && token->is<Operator>());
	EXPECT(token->get<Operator>().type() == OperatorType::Caret);

	token = tokenizer.scan_next();
	ASSERT(token && token->is<Eof>());
}

TEST(tokenizer_invalid_operator) {
	auto tokenizer = detail::Tokenizer{"$"};
	auto token = tokenizer.scan_next();
	EXPECT(!token);
}
} // namespace
