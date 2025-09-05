module;

#include <karm-math/au.h>

export module Vaev.Engine:layout.floats;

import :values;

namespace Vaev::Layout {

export struct FloatsContainer {
    struct Placed {
        RectAu rect;
        Float side;
    };

    Vec<Placed> _placed;
    Vec2Au _origin;
    Au _inlineSize;

    FloatsContainer(Vec2Au origin, Au inlineSize)
        : _origin(origin), _inlineSize(inlineSize) {}

    FloatsContainer() : _origin({0_au, 0_au}), _inlineSize(0_au) {}

    static bool _verticallyOverlaps(RectAu const& r, Au bandY, Au bandH) {
        Au r0 = r.y;
        Au r1 = r.y + r.height;
        Au b0 = bandY;
        Au b1 = bandY + bandH;
        return not(b1 <= r0 or r1 <= b0);
    }

    // Compute available inline range [start, end) in container coords at band y..y+h
    Pair<Au> inlineAvailable(Au y, Au h) const {
        // Fast path: no floats
        if (_placed.len() == 0)
            return {0_au, _inlineSize};

        Au left = 0_au;         // consumed from Start
        Au right = _inlineSize; // open end (exclusive)
        for (auto const& pf : _placed) {
            if (not _verticallyOverlaps(pf.rect, _origin.y + y, h))
                continue;
            if (pf.side == Float::INLINE_START or pf.side == Float::LEFT) {
                Au used = (pf.rect.x + pf.rect.width) - _origin.x;
                if (used > left)
                    left = used;
            } else if (pf.side == Float::INLINE_END or pf.side == Float::RIGHT) {
                Au used = pf.rect.x - _origin.x;
                if (used < right)
                    right = used;
            }
        }
        if (left > right)
            left = right; // no space => collapsed interval
        return {left, right};
    }

    Au clear(Clear clr, Au fromY) const {
        bool start = clr == Clear::BOTH or clr == Clear::INLINE_START or clr == Clear::LEFT;
        bool end = clr == Clear::BOTH or clr == Clear::INLINE_END or clr == Clear::RIGHT;

        Au y = fromY;
        bool changed = true;
        while (changed) {
            changed = false;
            for (auto const& pf : _placed) {
                if (not _verticallyOverlaps(pf.rect, _origin.y + y, 1_au))
                    continue;
                bool isStart = (pf.side == Float::INLINE_START or pf.side == Float::LEFT);
                bool isEnd = (pf.side == Float::INLINE_END or pf.side == Float::RIGHT);
                if ((start and isStart) or (end and isEnd)) {
                    Au ny = (pf.rect.y + pf.rect.height) - _origin.y;
                    if (ny > y) {
                        y = ny;
                        changed = true;
                    }
                }
            }
        }
        return y;
    }

    // Place a float box with size (inline=w, block=h) at or after y
    RectAu place(Vec2Au size, Float side, Au y) {
        Au w = size.x;
        Au h = size.y;
        if (w <= 0_au or h <= 0_au)
            return {_origin.x, _origin.y + y, 0_au, 0_au};

        Au curY = y;
        while (true) {
            auto [availStart, availEnd] = inlineAvailable(curY, h);
            Au availW = availEnd - availStart;
            if (availW >= w) {
                Au x = (side == Float::INLINE_START or side == Float::LEFT)
                           ? (_origin.x + availStart)
                           : (_origin.x + (availEnd - w));
                RectAu rect{x, _origin.y + curY, w, h};
                _placed.pushBack({rect, side});
                return rect;
            }

            // Not enough room on this slice; push below the lowest overlapping edge
            Au nextY = curY;
            for (auto const& pf : _placed) {
                if (_verticallyOverlaps(pf.rect, _origin.y + curY, h)) {
                    Au ny = (pf.rect.y + pf.rect.height) - _origin.y;
                    if (ny > nextY)
                        nextY = ny;
                }
            }
            if (nextY == curY) {
                // Wider than container: clamp to container width and place at side
                Au clampedW = (w > _inlineSize) ? _inlineSize : w;
                Au x = (side == Float::INLINE_START or side == Float::LEFT)
                           ? _origin.x
                           : _origin.x + (_inlineSize - clampedW);
                RectAu rect{x, _origin.y + curY, clampedW, h};
                _placed.pushBack({rect, side});
                return rect;
            }
            curY = nextY;
        }
    }
};

} // namespace Vaev::Layout