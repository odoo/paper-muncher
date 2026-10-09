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

    static LogicalSize fromPhysical(Math::Vec2<T> physical, WritingMode writingMode) {
        if (writingMode == WritingMode::HORIZONTAL_TB) {
            return {physical.width, physical.height};
        } else {
            return {physical.height, physical.width};
        }
    }

    Math::Vec2<T> toPhysical(WritingMode writingMode) const {
        if (writingMode == WritingMode::HORIZONTAL_TB) {
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

    Math::Insets<T> toPhysical(WritingMode writingMode, Direction direction) const {
        if (writingMode == WritingMode::HORIZONTAL_TB and direction == Direction::LTR) {
            return InsetsAu{
                blockStart,
                inlineEnd,
                blockEnd,
                inlineStart,
            };
        } else if (writingMode == WritingMode::HORIZONTAL_TB and direction == Direction::RTL) {
            return InsetsAu{
                blockStart,
                inlineStart,
                blockEnd,
                inlineEnd,
            };
        } else if (writingMode == WritingMode::VERTICAL_RL and direction == Direction::LTR) {
            return InsetsAu{
                inlineStart,
                blockStart,
                inlineEnd,
                blockEnd,
            };
        } else if (writingMode == WritingMode::VERTICAL_LR and direction == Direction::LTR) {
            return InsetsAu{
                inlineStart,
                blockEnd,
                inlineEnd,
                blockStart,
            };
        } else {
            logWarn("unsupported (writing-mode,direction): ({},{})", writingMode, direction);
            return InsetsAu{
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

export Size const& logicalWidth(Style::ComputedValues const& style) {
    if (style.writingMode == WritingMode::HORIZONTAL_TB) {
        return style.sizing->width;
    } else {
        return style.sizing->height;
    }
}

export Size const& logicalHeight(Style::ComputedValues const& style) {
    if (style.writingMode == WritingMode::HORIZONTAL_TB) {
        return style.sizing->height;
    } else {
        return style.sizing->width;
    }
}

} // namespace Vaev::Layout2
