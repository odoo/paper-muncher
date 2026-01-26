module;

#include <karm/macros>

export module Vaev.Engine:props.positioned;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// MARK: Positioning -----------------------------------------------------------

// https://www.w3.org/TR/CSS22/visuren.html#positioning-scheme
export struct PositionProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::POSITION;
        }

        Rc<Property> initial() const override {
            return makeRc<PositionProperty>(self(), Position{Keywords::STATIC});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<PositionProperty>(self(), c.position);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PositionProperty>(self(), try$(parseValue<Position>(c))));
        }
    };

    Position _value;

    PositionProperty(Rc<Property::Registration> registration, Position value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.position = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-top
export struct TopProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::TOP;
        }

        Rc<Property> initial() const override {
            return makeRc<TopProperty>(self(), Width{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<TopProperty>(self(), c.offsets->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TopProperty>(self(), try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    TopProperty(Rc<Property::Registration> registration, Width value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.offsets.cow().top = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-right
export struct RightProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::RIGHT;
        }

        Rc<Property> initial() const override {
            return makeRc<RightProperty>(self(), Width{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<RightProperty>(self(), c.offsets->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<RightProperty>(self(), try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    RightProperty(Rc<Property::Registration> registration, Width value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.offsets.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-bottom
export struct BottomProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BOTTOM;
        }

        Rc<Property> initial() const override {
            return makeRc<BottomProperty>(self(), Width{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BottomProperty>(self(), c.offsets->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BottomProperty>(self(), try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    BottomProperty(Rc<Property::Registration> registration, Width value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.offsets.cow().bottom = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-left
export struct LeftProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::LEFT;
        }

        Rc<Property> initial() const override {
            return makeRc<LeftProperty>(self(), Width{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<LeftProperty>(self(), c.offsets->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<LeftProperty>(self(), try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    LeftProperty(Rc<Property::Registration> registration, Width value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.offsets.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css2/#z-index
export struct ZIndexProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::Z_INDEX;
        }

        Rc<Property> initial() const override {
            return makeRc<ZIndexProperty>(self(), ZIndex{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ZIndexProperty>(self(), c.zIndex);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ZIndexProperty>(self(), try$(parseValue<ZIndex>(c))));
        }
    };

    ZIndex _value;

    ZIndexProperty(Rc<Property::Registration> registration, ZIndex value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.zIndex = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
