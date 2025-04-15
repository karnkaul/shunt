#pragma once
#include <shunt/term.hpp>
#include <cassert>
#include <cmath>
#include <unordered_map>

namespace shunt {
using CallFunc = double (*)(double x);
using BinOpFunc = double (*)(double a, double b);

struct CallArgs {
	Call call{};
	double x{};
};

struct BinOpArgs {
	BinaryOp op{};
	double a{};
	double b{};
};

class Eval {
  public:
	explicit Eval() {
		m_bin_op_funcs.at(BinaryOp::Plus) = [](double const a, double const b) { return a + b; };
		m_bin_op_funcs.at(BinaryOp::Minus) = [](double const a, double const b) { return a - b; };
		m_bin_op_funcs.at(BinaryOp::Star) = [](double const a, double const b) { return a * b; };
		m_bin_op_funcs.at(BinaryOp::Slash) = [](double const a, double const b) { return a / b; };
		m_bin_op_funcs.at(BinaryOp::Caret) = [](double const a, double const b) {
			return std::pow(a, b);
		};

		add_call_func("abs", [](double const x) { return std::abs(x); });
	}

	void add_call_func(Call const call, CallFunc const func) {
		m_call_funcs.insert_or_assign(call, func);
	}

	[[nodiscard]] auto evaluate(CallArgs const& args, double& out_result) const -> bool {
		auto const it = m_call_funcs.find(args.call);
		if (it == m_call_funcs.end()) { return false; }
		assert(it->second);
		out_result = it->second(args.x);
		return true;
	}

	void evaluate(BinOpArgs const& args, double& out_result) const {
		assert(args.op >= BinaryOp{} && args.op < BinaryOp::COUNT_);
		out_result = m_bin_op_funcs.at(args.op)(args.a, args.b);
	}

  private:
	std::unordered_map<Call, CallFunc> m_call_funcs{};
	EnumArray<BinaryOp, BinOpFunc> m_bin_op_funcs{};
};
} // namespace shunt
