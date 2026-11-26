export module Vaev.Engine:layout2.base;

import :layout;

namespace Vaev::Layout2 {

using Au = Layout::Au;
using Vec2Au = Layout::Vec2Au;
using InsetsAu = Layout::InsetsAu;

// https://www.w3.org/TR/css-layout-api-1/#intrinsic-sizes
struct IntrinsicSizes {
    Au minContentSize;
    Au maxContentSize;
};

// https://www.w3.org/TR/css-sizing-3/#contributions
struct IntrinsicContributions {
    Au min;
    Au max;
};

struct PendingMargin {
    Au positive = 0_au;
    Au negative = 0_au;

    void add(Au value) {
        if (value > 0_au) {
            positive = max(value, positive);
        } else {
            negative = min(value, negative);
        }
    }

    Au sum() const {
        return positive + negative;
    }

    void repr(Io::Emit& e) const {
        e("(pendingMargin pos={} neg={})", positive, negative);
    }
};

template <typename T>
struct LogicalSize {
    T inline_;
    T block;

    LogicalSize transpose() const {
        return {block, inline_};
    }

    static LogicalSize fromPhysical(Math::Vec2<T> physical, Style::WritingMode writingMode) {
        if (writingMode == Keywords::HORIZONTAL_TB) {
            return {physical.width, physical.height};
        } else {
            return {physical.height, physical.width};
        }
    }

    Math::Vec2<T> toPhysical(Style::WritingMode writingMode) const {
        if (writingMode == Keywords::HORIZONTAL_TB) {
            return Math::Vec2<T>{inline_, block};
        } else {
            return Math::Vec2<T>{block, inline_};
        }
    }



    void repr(Io::Emit& e) const {
        e("(logicalSize {} {})", inline_, block);
    }
};

template <typename T>
struct LogicalInsets {
    T inlineStart;
    T inlineEnd;
    T blockStart;
    T blockEnd;

    static LogicalInsets all(T const& value) {
        return {value, value, value, value};
    }

    T inlineSum() const {
        return inlineStart + inlineEnd;
    }

    T blockSum() const {
        return blockStart + blockEnd;
    }

    constexpr auto map(auto f) const {
        using U = decltype(f(inlineStart));
        return LogicalInsets<U>{
            f(inlineStart),
            f(inlineEnd),
            f(blockStart),
            f(blockEnd),
        };
    }

    Math::Insets<T> toPhysical(Style::WritingMode writingMode, Style::Direction direction) const {
        if (writingMode == Keywords::HORIZONTAL_TB and direction == Keywords::LTR) {
            return Math::Insets{
                blockStart,
                inlineEnd,
                blockEnd,
                inlineStart,
            };
        } else if (writingMode == Keywords::HORIZONTAL_TB and direction == Keywords::RTL) {
            return Math::Insets{
                blockStart,
                inlineStart,
                blockEnd,
                inlineEnd,
            };
        } else if (writingMode == Keywords::VERTICAL_RL and direction == Keywords::LTR) {
            return Math::Insets{
                inlineStart,
                blockStart,
                inlineEnd,
                blockEnd,
            };
        } else if (writingMode == Keywords::VERTICAL_LR and direction == Keywords::LTR) {
            return Math::Insets{
                inlineStart,
                blockEnd,
                inlineEnd,
                blockStart,
            };
        } else {
            logWarn("unsupported (writing-mode,direction): ({},{})", writingMode, direction);
            return Math::Insets{
                blockStart,
                inlineEnd,
                blockEnd,
                inlineStart,
            };
        }
    }

    void repr(Io::Emit& e) const {
        e("(logicalInsets inlineStart={} inlineEnd={} blockStart={} blockEnd={})", inlineStart, inlineEnd, blockStart, blockEnd);
    }
};

}

