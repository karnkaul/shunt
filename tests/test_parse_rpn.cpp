#include <detail/parser.hpp>
#include <detail/rpn_sink.hpp>
#include <klib/unit_test.hpp>
#include <nearly_eq.hpp>
#include <array>
#include <span>

namespace {
using namespace shunt;

struct Fixture {
	void parse_all(std::span<Token const> tokens_) {
		for (auto const token : tokens_) {
			auto result = parser.parse_next(token);
			ASSERT(result);
		}

		EXPECT(tokens.back().is<Eof>());
	}

	std::vector<Token> tokens{};
	std::vector<Token> stack{};
	detail::RpnSink sink{tokens};
	detail::Parser parser{stack, sink};
};

TEST(parse_rpn_binary) {
	static constexpr auto tokens = std::array{
		Token{.type = 1.0},
		Token{.type = Operator::Type::Plus},
		Token{.type = 2.0},
		Token{.type = Eof{}},
	};

	auto fixture = Fixture{};
	fixture.parse_all(tokens);
	ASSERT(fixture.tokens.size() == 4);
	ASSERT(fixture.tokens[0].is<Operand>());
	EXPECT(nearly_eq(fixture.tokens[0].get<Operand>(), 1.0));
	ASSERT(fixture.tokens[1].is<Operand>());
	EXPECT(nearly_eq(fixture.tokens[1].get<Operand>(), 2.0));
	ASSERT(fixture.tokens[2].is<Operator>());
	EXPECT(fixture.tokens[2].get<Operator>().type() == OperatorType::Plus);
}

TEST(parse_rpn_multi_binary) {
	static constexpr auto tokens = std::array{
		Token{.type = 2.0}, Token{.type = Operator::Type::Minus},
		Token{.type = 3.0}, Token{.type = Operator::Type::Star},
		Token{.type = 4.0}, Token{.type = Eof{}},
	};

	auto fixture = Fixture{};
	fixture.parse_all(tokens);
	ASSERT(fixture.tokens.size() == 6);
	ASSERT(fixture.tokens[0].is<Operand>());
	EXPECT(nearly_eq(fixture.tokens[0].get<Operand>(), 2.0));
	ASSERT(fixture.tokens[1].is<Operand>());
	EXPECT(nearly_eq(fixture.tokens[1].get<Operand>(), 3.0));
	ASSERT(fixture.tokens[2].is<Operand>());
	EXPECT(nearly_eq(fixture.tokens[2].get<Operand>(), 4.0));
	ASSERT(fixture.tokens[3].is<Operator>());
	EXPECT(fixture.tokens[3].get<Operator>().type() == OperatorType::Star);
	ASSERT(fixture.tokens[4].is<Operator>());
	EXPECT(fixture.tokens[4].get<Operator>().type() == OperatorType::Minus);
}

TEST(parse_rpn_parens) {
	static constexpr auto tokens = std::array{
		Token{.type = Paren::Left},
		Token{.type = 2.0},
		Token{.type = Operator::Type::Minus},
		Token{.type = 3.0},
		Token{.type = Paren::Right},
		Token{.type = Operator::Type::Star},
		Token{.type = 4.0},
		Token{.type = Eof{}},
	};

	auto fixture = Fixture{};
	fixture.parse_all(tokens);
	ASSERT(fixture.tokens.size() == 6);
	ASSERT(fixture.tokens[0].is<Operand>());
	EXPECT(nearly_eq(fixture.tokens[0].get<Operand>(), 2.0));
	ASSERT(fixture.tokens[1].is<Operand>());
	EXPECT(nearly_eq(fixture.tokens[1].get<Operand>(), 3.0));
	ASSERT(fixture.tokens[2].is<Operator>());
	EXPECT(fixture.tokens[2].get<Operator>().type() == OperatorType::Minus);
	ASSERT(fixture.tokens[3].is<Operand>());
	EXPECT(nearly_eq(fixture.tokens[3].get<Operand>(), 4.0));
	ASSERT(fixture.tokens[4].is<Operator>());
	EXPECT(fixture.tokens[4].get<Operator>().type() == OperatorType::Star);
}

TEST(parse_rpn_precedence) {
	static constexpr auto tokens = std::array{
		Token{.type = 2.0}, Token{.type = Operator::Type::Caret},
		Token{.type = 3.0}, Token{.type = Operator::Type::Slash},
		Token{.type = 4.0}, Token{.type = Operator::Type::Plus},
		Token{.type = 5.0}, Token{.type = Eof{}},
	};

	auto fixture = Fixture{};
	fixture.parse_all(tokens);
	ASSERT(fixture.tokens.size() == 8);
	ASSERT(fixture.tokens[0].is<Operand>());
	EXPECT(nearly_eq(fixture.tokens[0].get<Operand>(), 2.0));
	ASSERT(fixture.tokens[1].is<Operand>());
	EXPECT(nearly_eq(fixture.tokens[1].get<Operand>(), 3.0));
	ASSERT(fixture.tokens[2].is<Operator>());
	EXPECT(fixture.tokens[2].get<Operator>().type() == OperatorType::Caret);
	ASSERT(fixture.tokens[3].is<Operand>());
	EXPECT(nearly_eq(fixture.tokens[3].get<Operand>(), 4.0));
	ASSERT(fixture.tokens[4].is<Operator>());
	EXPECT(fixture.tokens[4].get<Operator>().type() == OperatorType::Slash);
	ASSERT(fixture.tokens[5].is<Operand>());
	EXPECT(nearly_eq(fixture.tokens[5].get<Operand>(), 5.0));
	ASSERT(fixture.tokens[6].is<Operator>());
	EXPECT(fixture.tokens[6].get<Operator>().type() == OperatorType::Plus);
}
} // namespace
