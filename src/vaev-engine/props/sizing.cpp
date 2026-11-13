module;

#include <karm/macros>

export module Vaev.Engine:props.sizing;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Sizing ----------------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export struct BoxSizingProperty : Property {
    static constexpr auto ID = LonghandId::BOX_SIZING;

    static Rc<Property> initial() {
        return makeRc<BoxSizingProperty>(Keywords::CONTENT_BOX);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BoxSizingProperty>(c.boxSizing);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<BoxSizingProperty>(try$(parseValue<BoxSizing>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BOX_SIZING) {}
    };

    BoxSizing _value;

    BoxSizingProperty(BoxSizing value)
        : Property(registrationOf<BoxSizingProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.boxSizing = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width
export struct WidthProperty : Property {
    static constexpr auto ID = LonghandId::WIDTH;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};

    static Rc<Property> initial() {
        return makeRc<WidthProperty>(Size{Keywords::AUTO});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<WidthProperty>(c.sizing->width);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<WidthProperty>(try$(parseValue<Size>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::WIDTH, FLAGS) {}

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            if (auto prop = LonghandRegistration::parsePresentationAttribute(style))
                return prop;
            return LonghandRegistration::parsePresentationAttribute(Io::format("{}px", style));
        }
    };

    Size _value;

    WidthProperty(Size value)
        : Property(registrationOf<WidthProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().width = _value;
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
    static constexpr auto ID = LonghandId::HEIGHT;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};

    static Rc<Property> initial() {
        return makeRc<HeightProperty>(Size{Keywords::AUTO});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<HeightProperty>(c.sizing->height);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<HeightProperty>(try$(parseValue<Size>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::HEIGHT, FLAGS) {}

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            if (auto prop = LonghandRegistration::parsePresentationAttribute(style))
                return prop;
            return LonghandRegistration::parsePresentationAttribute(Io::format("{}px", style));
        }
    };

    Size _value;

    HeightProperty(Size value)
        : Property(registrationOf<HeightProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().height = _value;
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
    static constexpr auto ID = LonghandId::MIN_WIDTH;

    static Rc<Property> initial() {
        return makeRc<MinWidthProperty>(Size{Keywords::AUTO});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MinWidthProperty>(c.sizing->minWidth);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<MinWidthProperty>(try$(parseValue<Size>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::MIN_WIDTH) {}
    };

    Size _value;

    MinWidthProperty(Size value)
        : Property(registrationOf<MinWidthProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().minWidth = _value;
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
    static constexpr auto ID = LonghandId::MIN_HEIGHT;

    static Rc<Property> initial() {
        return makeRc<MinHeightProperty>(Size{Keywords::AUTO});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MinHeightProperty>(c.sizing->minHeight);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<MinHeightProperty>(try$(parseValue<Size>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::MIN_HEIGHT) {}
    };

    Size _value;

    MinHeightProperty(Size value)
        : Property(registrationOf<MinHeightProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().minHeight = _value;
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
    static constexpr auto ID = LonghandId::MAX_WIDTH;

    static Rc<Property> initial() {
        return makeRc<MaxWidthProperty>(MaxSize{Keywords::NONE});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MaxWidthProperty>(c.sizing->maxWidth);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<MaxWidthProperty>(try$(parseValue<MaxSize>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::MAX_WIDTH) {}
    };

    MaxSize _value;

    MaxWidthProperty(MaxSize value)
        : Property(registrationOf<MaxWidthProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().maxWidth = _value;
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
    static constexpr auto ID = LonghandId::MAX_HEIGHT;

    static Rc<Property> initial() {
        return makeRc<MaxHeightProperty>(MaxSize{Keywords::NONE});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MaxHeightProperty>(c.sizing->maxHeight);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<MaxHeightProperty>(try$(parseValue<MaxSize>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::MAX_HEIGHT) {}
    };

    MaxSize _value;

    MaxHeightProperty(MaxSize value)
        : Property(registrationOf<MaxHeightProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.sizing.cow().maxHeight = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
