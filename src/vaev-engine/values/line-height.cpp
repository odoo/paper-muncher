module;

#include <karm-base/union.h>
#include <karm-io/emit.h>

export module Vaev.Engine:values.line_height;

import :css;
import :values.base;
import :values.length;
import :values.percent;
import :values.primitives;

namespace Vaev {

export struct LineHeight {
    struct _Normal {};

    static constexpr _Normal NORMAL = {};
    Union<PercentOr<Length>, Number> _value;

    LineHeight(_Normal) : _value(1.2) {}

    LineHeight(PercentOr<Length> value) : _value(value) {}

    LineHeight(Number value) : _value(value) {}

    void repr(Io::Emit& e) const {
        e("{}", _value);
    }
};

export template <>
struct ValueParser<LineHeight> {
    static Res<LineHeight> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("normal"))) {
            return Ok(LineHeight::NORMAL);
        }

        {
            auto rb = c.rollbackPoint();
            auto maybeNumber = parseValue<Number>(c);
            if (maybeNumber) {
                rb.disarm();
                return Ok(maybeNumber.unwrap());
            }
        }

        return Ok(LineHeight{try$(parseValue<PercentOr<Length>>(c))});
    }
};

} // namespace Vaev
