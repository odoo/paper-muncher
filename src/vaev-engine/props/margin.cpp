module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.margin;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// MARK: Margin ----------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-margin

export struct MarginTopProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_TOP;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginTopProperty>(self(), CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginTopProperty>(self(), c.margin->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginTopProperty>(self(), try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginTopProperty(Rc<Property::Registration> registration, Width value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.margin.cow().top = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginRightProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_RIGHT;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginRightProperty>(self(), CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginRightProperty>(self(), c.margin->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginRightProperty>(self(), try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginRightProperty(Rc<Property::Registration> registration, Width value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.margin.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginBottomProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_BOTTOM;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginBottomProperty>(self(), CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginBottomProperty>(self(), c.margin->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBottomProperty>(self(), try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginBottomProperty(Rc<Property::Registration> registration, Width value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.margin.cow().bottom = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginLeftProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_LEFT;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginLeftProperty>(self(), CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginLeftProperty>(self(), c.margin->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginLeftProperty>(self(), try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginLeftProperty(Rc<Property::Registration> registration, Width value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.margin.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<MarginProperty>(self(), Math::Insets<Width>{CalcValue<PercentOr<Length>>(Length{})});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginProperty>(self(), *c.margin);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginProperty>(self(), try$(parseValue<Math::Insets<Width>>(c))));
        }
    };

    Math::Insets<Width> _value;

    MarginProperty(Rc<Property::Registration> registration, Math::Insets<Width> value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
        return {
            makeRc<MarginTopProperty>(registry.resolveRegistration(Properties::MARGIN_TOP, {}).unwrap(), _value.top),
            makeRc<MarginBottomProperty>(registry.resolveRegistration(Properties::MARGIN_BOTTOM, {}).unwrap(), _value.bottom),
            makeRc<MarginLeftProperty>(registry.resolveRegistration(Properties::MARGIN_LEFT, {}).unwrap(), _value.start),
            makeRc<MarginRightProperty>(registry.resolveRegistration(Properties::MARGIN_RIGHT, {}).unwrap(), _value.end),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#margin-properties

export struct MarginInlineStartProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_INLINE_START;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginInlineStartProperty>(self(), CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginInlineStartProperty>(self(), c.margin->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginInlineStartProperty>(self(), try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginInlineStartProperty(Rc<Property::Registration> registration, Width value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginInlineEndProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_INLINE_END;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginInlineEndProperty>(self(), CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginInlineEndProperty>(self(), c.margin->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginInlineEndProperty>(self(), try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginInlineEndProperty(Rc<Property::Registration> registration, Width value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginInlineProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_INLINE;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginInlineProperty>(self(), Math::Insets<Width>{CalcValue<PercentOr<Length>>(Length{})});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginInlineProperty>(self(), Math::Insets{c.margin->start, c.margin->end});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginInlineProperty>(self(), try$(parseValue<Math::Insets<Width>>(c))));
        }
    };

    Math::Insets<Width> _value;

    MarginInlineProperty(Rc<Property::Registration> registration, Math::Insets<Width> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().start = _value.start;
        c.margin.cow().end = _value.end;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginBlockStartProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_BLOCK_START;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginBlockStartProperty>(self(), CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginBlockStartProperty>(self(), c.margin->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBlockStartProperty>(self(), try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginBlockStartProperty(Rc<Property::Registration> registration, Width value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().top = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginBlockEndProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_BLOCK_END;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginBlockEndProperty>(self(), CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginBlockEndProperty>(self(), c.margin->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBlockEndProperty>(self(), try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginBlockEndProperty(Rc<Property::Registration> registration, Width value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().bottom = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginBlockProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_BLOCK;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginBlockProperty>(self(), Math::Insets<Width>{CalcValue<PercentOr<Length>>(Length{})});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginBlockProperty>(self(), Math::Insets{c.margin->top, c.margin->bottom});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBlockProperty>(self(), try$(parseValue<Math::Insets<Width>>(c))));
        }
    };

    Math::Insets<Width> _value;

    MarginBlockProperty(Rc<Property::Registration> registration, Math::Insets<Width> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().top = _value.top;
        c.margin.cow().bottom = _value.bottom;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
