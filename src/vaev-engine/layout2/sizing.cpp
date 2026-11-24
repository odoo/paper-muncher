export module Vaev.Engine:layout2.sizing;

import :style;

import Karm.Math;
import Karm.Core;

namespace Vaev::Layout2 {

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

export Style::Size const& logicalWidth(Style::ComputedValues const& style) {
    if (style.writingMode == Keywords::HORIZONTAL_TB) {
        return style.sizing->width;
    } else {
        return style.sizing->height;
    }
}

export Style::Size const& logicalHeight(Style::ComputedValues const& style) {
    if (style.writingMode == Keywords::HORIZONTAL_TB) {
        return style.sizing->height;
    } else {
        return style.sizing->width;
    }
}

} // namespace Vaev::Layout2
