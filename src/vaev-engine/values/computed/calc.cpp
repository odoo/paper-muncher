export module Vaev.Engine:values.computed.calc;

import Karm.Core;

using namespace Karm;

namespace Vaev::Experimental {

#define FOREACH_INSTRUCTION(INS)     \
    INS(NOP, 0x0, 1, 0, 0)           \
    INS(CONST_NUMBER, 0x1, 9, 0, 1)  \
    INS(CONST_PERCENT, 0x2, 9, 0, 1) \
    INS(CONST_0, 0x3, 2, 0, 1)       \
    INS(ADD, 0x10, 1, 2, 1)          \
    INS(NEG, 0x11, 1, 2, 1)          \
    INS(MUL, 0x12, 1, 2, 1)          \
    INS(INV, 0x13, 1, 2, 1)          \
    INS(MIN, 0x20, 1, 2, 1)          \
    INS(MAX, 0x21, 1, 2, 1)

namespace Ins {

#define ITER(NAME, OPCODE, ...) constexpr u8 NAME = OPCODE;
FOREACH_INSTRUCTION(ITER)
#undef ITER

struct Info {
    u8 width;
    u8 inputs;
    u8 outputs;
};

constexpr Info const& info(u8 opcode) {
    static constexpr Info table[255] = {
#define ITER(_NAME, OPCODE, WIDTH, INPUTS, OUTPUTS) [OPCODE] = {WIDTH, INPUTS, OUTPUTS},
        FOREACH_INSTRUCTION(ITER)
#undef ITER
    };

    return table[opcode];
}

// enum : u8 {
//     PUSH_NUMBER,
//     PUSH_PERCENT,
//
//     // Arithmetic
//     ADD,
//     SUB,
//     MUL,
//     DIV,
//
//     // Functions
//     MIN,
//     MAX,
//
//     CLAMP,
//
//     // TODO: Different round strategies
//     ROUND,
//     ROUND2,
//
//     MOD,
//     REM,
//     SIN,
//     COS,
//     TAN,
//     ASIN,
//     ACOS,
//     ATAN,
//     ATAN2,
//     POW,
//     SQRT,
//     HYPOT,
//     LOG,
//     EXP,
//     ABS,
//     SIGN,
// };
} // namespace Ins

namespace CalcOps {

f64 add(f64 a, f64 b) { return a + b; }

f64 neg(f64 a) { return -a; }

f64 mul(f64 a, f64 b) { return a * b; }

f64 inv(f64 a) { return 1.0 / a; }

f64 min(f64 a, f64 b) { return min(a, b); }

f64 max(f64 a, f64 b) { return max(a, b); }

f64 clamp(f64 val, f64 min, f64 max) { clamp(val, min, max); }

} // namespace CalcOps

struct Calc {
    SmallVec<u8, Ins::info(Ins::CONST_NUMBER).width * 2 + Ins::info(Ins::ADD).width> bytecode;

    f64 execute(f64 relativeTo) {
        usize pc = 0;
        InlineVec<f64, 16> stack;

        auto execUnop = [&]<typename F>(F f) {
            f64 arg1 = stack.popBack();
            stack.pushBack(f(arg1));
        };

        auto execBinop = [&]<typename F>(F f) {
            f64 arg1 = stack.popBack();
            f64 arg2 = stack.popBack();
            stack.pushBack(f(arg1, arg2));
        };

        auto execTernop = [&]<typename F>(F f) {
            f64 arg1 = stack.popBack();
            f64 arg2 = stack.popBack();
            f64 arg3 = stack.popBack();
            stack.pushBack(f(arg1, arg2, arg3));
        };

        while (pc < bytecode.len()) {
            switch (bytecode[pc++]) {
            case Ins::CONST_NUMBER: {
                f64 val;
                std::memcpy(&val, &bytecode[pc], sizeof(f64));
                pc += sizeof(f64);
                stack.pushBack(val);
                break;
            }
            case Ins::CONST_PERCENT: {
                f64 val;
                std::memcpy(&val, &bytecode[pc], sizeof(f64));
                pc += sizeof(f64);
                stack.pushBack(val * relativeTo / 100.0);
                break;
            }

            case Ins::ADD:
                execBinop(CalcOps::add);
                break;
            case Ins::NEG:
                execUnop(CalcOps::neg);
                break;
            case Ins::MUL:
                execBinop(CalcOps::mul);
                break;
            case Ins::INV:
                execUnop(CalcOps::inv);
                break;

            case Ins::MIN:
                execBinop(CalcOps::min);
                break;
            case Ins::MAX:
                execBinop(CalcOps::max);
                break;

            // case Ins::CLAMP:
            //     execTernop([](f64 a, f64 b, f64 c) {
            //         return clamp(a, b, c);
            //     });
            //     break;
            //
            // case Ins::ROUND:
            //     execUnop([](f64 a) {
            //         return Math::round(a);
            //     });
            //     break;
            // case Ins::ROUND2:
            //     execBinop([](f64 a, f64 b) {
            //         return a * Math::round(a / b);
            //     });
            //     break;
            //
            // case Ins::MOD:
            //     execBinop([](f64 a, f64 b) {
            //         return Math::fmod(a, b);
            //     });
            //     break;
            default:
                notImplemented();
            }
        }

        return stack.popBack();
    }
};

} // namespace Vaev::Experimental
