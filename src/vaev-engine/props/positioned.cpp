module;

#include <karm/macros>

export module Vaev.Engine:props.positioned;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Positioning -----------------------------------------------------------

// https://www.w3.org/TR/CSS22/visuren.html#positioning-scheme
export struct PositionProperty : Property {
    static constexpr auto ID = LonghandId::POSITION;

    static Rc<Property> initial() {
        return makeRc<PositionProperty>(Position{Keywords::STATIC});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PositionProperty>(c.position);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<PositionProperty>(try$(parseValue<Position>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::POSITION) {}
    };

    Position _value;

    PositionProperty(Position value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.position = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-top
export struct TopProperty : Property {
    static constexpr auto ID = LonghandId::TOP;

    using Value = Union<Keywords::Auto, Calc<PercentOr<Length>>>;

    static Rc<Property> initial() {
        return makeRc<TopProperty>(Value{Keywords::AUTO});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<TopProperty>(c.insets->top);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<TopProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::TOP) {}
    };

    Value _value;

    TopProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.insets.cow().top = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-right
export struct RightProperty : Property {
    static constexpr auto ID = LonghandId::RIGHT;

    using Value = TopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<RightProperty>(Value{Keywords::AUTO});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<RightProperty>(c.insets->end);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<RightProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::RIGHT) {}
    };

    Value _value;

    RightProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.insets.cow().end = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-bottom
export struct BottomProperty : Property {
    static constexpr auto ID = LonghandId::BOTTOM;

    using Value = TopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<BottomProperty>(Value{Keywords::AUTO});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BottomProperty>(c.insets->bottom);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BottomProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BOTTOM) {}
    };

    Union<Keywords::Auto, Calc<PercentOr<Length>>> _value;

    BottomProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.insets.cow().bottom = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-left
export struct LeftProperty : Property {
    static constexpr auto ID = LonghandId::LEFT;

    using Value = TopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<LeftProperty>(Value{Keywords::AUTO});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<LeftProperty>(c.insets->start);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<LeftProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::LEFT) {}
    };

    Value _value;

    LeftProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.insets.cow().start = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css2/#z-index
export struct ZIndexProperty : Property {
    static constexpr auto ID = LonghandId::Z_INDEX;

    static Rc<Property> initial() {
        return makeRc<ZIndexProperty>(ZIndex{Keywords::AUTO});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ZIndexProperty>(c.zIndex);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<ZIndexProperty>(try$(parseValue<ZIndex>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::Z_INDEX) {}
    };

    ZIndex _value;

    ZIndexProperty(ZIndex value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.zIndex = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
