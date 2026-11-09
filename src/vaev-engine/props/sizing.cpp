module;

#include <karm/macros>

export module Vaev.Engine:props.sizing;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;

namespace Vaev::Style {

// MARK: Sizing ----------------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export struct BoxSizingProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BOX_SIZING) {}

        Rc<Property> initial() const override {
            return makeRc<BoxSizingProperty>(self(), Keywords::CONTENT_BOX);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BoxSizingProperty>(self(), c.boxSizing);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BoxSizingProperty>(self(), try$(parseValue<BoxSizing>(c))));
        }
    };

    BoxSizing _value;

    BoxSizingProperty(Rc<Property::Registration> registration, BoxSizing value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.boxSizing = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width
export struct WidthProperty : Property {
    using Value = Union<Keywords::Auto, Css::LengthPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::WIDTH, PRESENTATION_ATTRIBUTE) {}

        Rc<Property> initial() const override {
            return makeRc<WidthProperty>(self(), Value{Keywords::AUTO});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<WidthProperty>(self(), toSpecified(c.sizing->width));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            if (auto prop = Property::Registration::parsePresentationAttribute(style))
                return prop;
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<WidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    WidthProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().width = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-height

export struct HeightProperty : Property {
    using Value = WidthProperty::Value;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::HEIGHT, PRESENTATION_ATTRIBUTE) {}

        Rc<Property> initial() const override {
            return makeRc<HeightProperty>(self(), Value{Keywords::AUTO});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<HeightProperty>(self(), toSpecified(c.sizing->height));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            if (auto prop = Property::Registration::parsePresentationAttribute(style))
                return prop;
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<HeightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    HeightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().height = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-width
export struct MinWidthProperty : Property {
    using Value = WidthProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MIN_WIDTH) {}

        Rc<Property> initial() const override {
            return makeRc<MinWidthProperty>(self(), Value{Keywords::AUTO});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MinWidthProperty>(self(), toSpecified(c.sizing->minWidth));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MinWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MinWidthProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().minWidth = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-height
export struct MinHeightProperty : Property {
    using Value = WidthProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MIN_HEIGHT) {}

        Rc<Property> initial() const override {
            return makeRc<MinHeightProperty>(self(), Value{Keywords::AUTO});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MinHeightProperty>(self(), toSpecified(c.sizing->minHeight));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MinHeightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MinHeightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().minHeight = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-width

export struct MaxWidthProperty : Property {
    using Value = Union<Keywords::None, Css::LengthPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MAX_WIDTH) {}

        Rc<Property> initial() const override {
            return makeRc<MaxWidthProperty>(self(), Value{Keywords::NONE});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MaxWidthProperty>(self(), toSpecified(c.sizing->maxWidth));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MaxWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MaxWidthProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().maxWidth = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-height
export struct MaxHeightProperty : Property {
    using Value = MaxWidthProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MAX_HEIGHT) {}

        Rc<Property> initial() const override {
            return makeRc<MaxHeightProperty>(self(), Value{Keywords::NONE});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MaxHeightProperty>(self(), toSpecified(c.sizing->maxHeight));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MaxHeightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MaxHeightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().maxHeight = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
