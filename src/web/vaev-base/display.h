#pragma once

#include <karm-base/panic.h>
#include <karm-io/emit.h>

namespace Vaev {

// 2. MARK: Box Layout Modes: the display property
// https://www.w3.org/TR/css-display-3/#the-display-properties

struct Display {
    enum struct Box : u8 {
        CONTENTS,
        NONE,

        _LEN0,
    };

    using enum Box;

    enum struct Internal : u8 {
        TABLE_ROW_GROUP,
        TABLE_HEADER_GROUP,
        TABLE_FOOTER_GROUP,
        TABLE_ROW,
        TABLE_CELL,
        TABLE_COLUMN_GROUP,
        TABLE_COLUMN,
        TABLE_CAPTION,
        TABLE_BOX,
        RUBY_BASE,
        RUBY_TEXT,
        RUBY_BASE_CONTAINER,
        RUBY_TEXT_CONTAINER,

        _LEN1,
    };

    bool isHeadBodyFootOrRow() {
        return (
            *this == TABLE_HEADER_GROUP or
            *this == TABLE_ROW_GROUP or
            *this == TABLE_FOOTER_GROUP or
            *this == TABLE_ROW
        );
    }

    bool isHeadBodyFootRowOrColGroup() {
        return (
            isHeadBodyFootOrRow() or
            *this == TABLE_COLUMN_GROUP
        );
    }

    using enum Internal;

    enum struct Inside : u8 {
        FLOW,
        FLOW_ROOT,
        FLEX,
        GRID,
        RUBY,
        TABLE,
        MATH,

        _LEN2,
    };

    using enum Inside;

    enum struct Outside : u8 {
        BLOCK,
        INLINE,
        RUN_IN,

        _LEN3,
    };

    using enum Outside;

    enum struct Item : u8 {
        NO,
        YES,

        _LEN4,
    };

    enum struct Type : u8 {
        DEFAULT,
        INTERNAL,
        BOX,

        _LEN5,
    };

    using enum Type;

    Type _type;

    union {
        struct {
            Inside _inside;
            Outside _outside;
            Item _item;
        };

        Box _box;
        Internal _internal;
    };

    constexpr Display()
        : Display(Display::FLOW, Display::INLINE) {}

    constexpr Display(Box box)
        : _type(Type::BOX), _box(box) {}

    constexpr Display(Internal internal)
        : _type(Type::INTERNAL), _internal(internal) {}

    constexpr Display(Inside inside, Outside outside, Item item = Item::NO)
        : _type(Type::DEFAULT), _inside(inside), _outside(outside), _item(item) {}

    constexpr bool is(Type type) const {
        return _type == type;
    }

    constexpr Type type() const {
        return _type;
    }

    constexpr bool operator==(Type type) const {
        return _type == type;
    }

    Inside inside() const {
        if (not is(Type::DEFAULT))
            panic("invalid display type");
        return _inside;
    }

    bool operator==(Inside inside) const {
        return is(Type::DEFAULT) and _inside == inside;
    }

    Outside outside() const {
        if (not is(Type::DEFAULT))
            panic("invalid display type");
        return _outside;
    }

    bool operator==(Outside outside) const {
        return is(Type::DEFAULT) and _outside == outside;
    }

    Item item() const {
        if (not is(Type::DEFAULT))
            panic("invalid display type");
        return _item;
    }

    bool operator==(Item item) const {
        return is(Type::DEFAULT) and _item == item;
    }

    Box box() const {
        if (not is(Type::BOX))
            panic("invalid display type");
        return _box;
    }

    bool operator==(Box box) const {
        return is(Type::BOX) and _box == box;
    }

    Internal internal() const {
        if (not is(Type::INTERNAL))
            panic("invalid display type");
        return _internal;
    }

    bool operator==(Internal internal) const {
        return is(Type::INTERNAL) and _internal == internal;
    }

    bool operator==(Display const& other) const {
        if (_type != other._type)
            return false;

        switch (_type) {
        case Type::DEFAULT:
            return _inside == other._inside and _outside == other._outside and _item == other._item;

        case Type::INTERNAL:
            return _internal == other._internal;

        case Type::BOX:
            return _box == other._box;

        default:
            unreachable();
        }

        return false;
    }

    void repr(Io::Emit& e) const {
        switch (_type) {
        case Type::DEFAULT:
            e("(display inside: {}, outside: {}, item: {})", _inside, _outside, _item);
            break;

        case Type::INTERNAL:
            e("(display internal: {})", _internal);
            break;

        case Type::BOX:
            e("(display box: {})", _box);
            break;

        default:
            unreachable();
        }
    }

    always_inline bool isTableTrack() const {
        if (not is(Type::INTERNAL))
            return false;
        return _internal == TABLE_ROW or _internal == TABLE_COLUMN;
    }

    always_inline bool isTableTrackGroup() const {
        if (not is(Type::INTERNAL))
            return false;
        return _internal == TABLE_ROW_GROUP or _internal == TABLE_HEADER_GROUP or
               _internal == TABLE_FOOTER_GROUP or _internal == TABLE_COLUMN_GROUP;
    }

    always_inline bool isProperTableChild() const {
        if (not is(Type::INTERNAL))
            return false;
        return _internal == TABLE_CAPTION or isTableTrack() or isTableTrackGroup();
    }

    static Array constexpr const INTERNAL_TABLE = {
        TABLE_ROW_GROUP,
        TABLE_HEADER_GROUP,
        TABLE_FOOTER_GROUP,
        TABLE_ROW,
        TABLE_CELL,
        TABLE_COLUMN_GROUP,
        TABLE_COLUMN,
        TABLE_CAPTION,
        TABLE_BOX
    };

    bool isTableInternal() const {
        if (type() != INTERNAL)
            return false;
        return contains(INTERNAL_TABLE, _internal);
    }

    // https://www.w3.org/TR/css-display-3/#blockify
    Display blockify() {
        if (_type == BOX)
            return *this;

        if (_type == INTERNAL) {
            // If a layout-internal box is blockified, its inner display type converts to flow so that it becomes
            // a block container.
            // FIXME: in our representation, layout-internal does not have an inner display property
            panic("cannot blockify layout-internal display");
        }

        if (_outside == BLOCK)
            return *this;

        if (_inside == FLOW_ROOT) {
            return {
                FLOW,
                BLOCK,
                _item,
            };
        } else {
            return {
                _inside,
                BLOCK,
                _item,
            };
        }
    }

    // https://www.w3.org/TR/css-display-3/#inlinify
    Display inlinify() {
        if (_type != DEFAULT)
            return *this;

        if (_outside == INLINE)
            return *this;

        if (_inside == FLOW) {
            return {
                FLOW,
                BLOCK,
                _item,
            };
        } else {
            return {
                _inside,
                BLOCK,
                _item,
            };
        }
    }
};

} // namespace Vaev
