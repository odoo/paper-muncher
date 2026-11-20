module;

#include <karm/macros>

export module Vaev.Engine:props.sizing;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Sizing ----------------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export struct BoxSizingProperty : Property {
    using Value = Css::BoxSizing;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BOX_SIZING) {}

        InitialValue initialValue() const override {
            return "content-box"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BoxSizingProperty>(self(), c.boxSizing);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BoxSizingProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BoxSizingProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

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
    using Value = Union<
        Keywords::Auto,
        Css::LengthPercentage,
        Keywords::Stretch,
        Keywords::MinContent,
        Keywords::MaxContent,
        Keywords::FitContent>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::WIDTH, PRESENTATION_ATTRIBUTE) {}

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<WidthProperty>(self(), specifyValue<Value>(c.sizing->width));
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
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().width = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
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

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<HeightProperty>(self(), specifyValue<Value>(c.sizing->height));
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
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().height = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
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

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MinWidthProperty>(self(), specifyValue<Value>(c.sizing->minWidth));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MinWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MinWidthProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().minWidth = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
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

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MinHeightProperty>(self(), specifyValue<Value>(c.sizing->minHeight));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MinHeightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MinHeightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().minHeight = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
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

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MaxWidthProperty>(self(), specifyValue<Value>(c.sizing->maxWidth));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MaxWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MaxWidthProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().maxWidth = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
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

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MaxHeightProperty>(self(), specifyValue<Value>(c.sizing->maxHeight));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MaxHeightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MaxHeightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().maxHeight = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
