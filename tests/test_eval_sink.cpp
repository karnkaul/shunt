#include <detail/eval_sink.hpp>
#include <klib/unit_test.hpp>
#include <nearly_eq.hpp>

namespace {
using namespace shunt;

struct Fixture {
	CallTable call_table{default_call_table()};
	std::vector<Operand> operands{};
	detail::EvalSink sink{call_table, operands};
};

TEST(eval_sink_empty) {
	auto fixture = Fixture{};
	auto const result = fixture.sink.get_value();
	EXPECT(!result);
}

TEST(eval_sink_number) {
	auto fixture = Fixture{};
	fixture.sink.on_operand(Token{.type = 2.0});
	auto const result = fixture.sink.get_value();
	ASSERT(result);
	EXPECT(nearly_eq(*result, 2.0));
}

TEST(eval_sink_binary) {
	auto fixture = Fixture{};
	fixture.sink.on_operand(Token{.type = 2.0});
	fixture.sink.on_operand(Token{.type = 3.0});
	fixture.sink.on_operator(Token{.type = Operator::Type::Plus});
	auto result = fixture.sink.get_value();
	ASSERT(result);
	EXPECT(nearly_eq(*result, 5.0));

	fixture.sink.on_operand(Token{.type = 2.0});
	fixture.sink.on_operator(Token{.type = Operator::Type::Caret});
	result = fixture.sink.get_value();
	ASSERT(result);
	EXPECT(nearly_eq(*result, 25.0));
}

TEST(eval_sink_call) {
	auto fixture = Fixture{};
	fixture.sink.on_operand(Token{.type = -4.0});
	fixture.sink.on_call(Token{.type = Call{"abs"}});
	auto result = fixture.sink.get_value();
	ASSERT(result);
	EXPECT(nearly_eq(*result, 4.0));

	auto err_caught = false;
	try {
		fixture.sink.on_call(Token{.type = Call{"foo"}});
	} catch (SyntaxError const& /*s*/) { err_caught = true; }
	EXPECT(err_caught);
}
} // namespace
