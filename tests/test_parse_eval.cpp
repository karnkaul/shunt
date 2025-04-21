#include <detail/eval_sink.hpp>
#include <detail/parser.hpp>
#include <klib/unit_test.hpp>
#include <nearly_eq.hpp>
#include <span>

namespace {
using namespace shunt;

struct Fixture {
	void parse_all(std::span<Token const> tokens) {
		for (auto const token : tokens) {
			auto result = parser.parse_next(token);
			ASSERT(result);
		}
	}

	CallTable call_table{default_call_table()};
	std::vector<Token> stack{};
	std::vector<Operand> operands{};
	detail::EvalSink sink{call_table, operands};
	detail::Parser parser{stack, sink};
};

TEST(parse_eval_number) {
	static constexpr auto num_v = 1.23456;

	auto fixture = Fixture{};
	auto const parse_result = fixture.parser.parse_next(Token{.type = num_v});
	ASSERT(parse_result);
	auto const result = fixture.sink.get_value();
	ASSERT(result);
	EXPECT(nearly_eq(*result, num_v));
}

TEST(parse_eval_binary) {
	static constexpr auto tokens = std::array{
		Token{.type = 1.0},
		Token{.type = Operator::Type::Plus},
		Token{.type = 2.0},
		Token{.type = Eof{}},
	};

	auto fixture = Fixture{};
	fixture.parse_all(tokens);
	auto const result = fixture.sink.get_value();
	ASSERT(result);
	EXPECT(nearly_eq(*result, 3.0));
}

TEST(parse_eval_multi_binary) {
	static constexpr auto tokens = std::array{
		Token{.type = 2.0}, Token{.type = Operator::Type::Minus},
		Token{.type = 3.0}, Token{.type = Operator::Type::Star},
		Token{.type = 4.0}, Token{.type = Eof{}},
	};

	auto fixture = Fixture{};
	fixture.parse_all(tokens);
	auto const result = fixture.sink.get_value();
	ASSERT(result);
	EXPECT(nearly_eq(*result, -10.0));
}

TEST(parse_eval_parens) {
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
	auto const result = fixture.sink.get_value();
	ASSERT(result);
	EXPECT(nearly_eq(*result, -4.0));
}

TEST(parse_eval_precedence) {
	static constexpr auto tokens = std::array{
		Token{.type = 2.0}, Token{.type = Operator::Type::Caret},
		Token{.type = 3.0}, Token{.type = Operator::Type::Slash},
		Token{.type = 4.0}, Token{.type = Operator::Type::Plus},
		Token{.type = 5.0}, Token{.type = Eof{}},
	};

	auto fixture = Fixture{};
	fixture.parse_all(tokens);
	auto const result = fixture.sink.get_value();
	ASSERT(result);
	EXPECT(nearly_eq(*result, 7.0));
}
} // namespace
