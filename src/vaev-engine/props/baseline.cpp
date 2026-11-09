module;

#include <karm/macros>

export module Vaev.Engine:props.baseline;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-inline/#line-height-property
export struct LineHeightProperty : Property {
    using Value = Union<Keywords::Normal, Css::Number, Css::LengthPercentage>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::LINE_HEIGHT, INHERITED) {}

        Rc<Property> initial() const override {
            return makeRc<LineHeightProperty>(self(), Keywords::NORMAL);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.inline_.cow().lineHeight = parent.inline_->lineHeight;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<LineHeightProperty>(
                self(),
                c.inline_->lineHeight.visit(
                    [](Keywords::Normal const& normal) -> Value {
                        return normal;
                    },
                    [](f64 const& number) -> Value {
                        return Css::Number::fromCanonical(number);
                    },
                    [](Px const& px) -> Value {
                        return toSpecified(PxPercent{px});
                    }
                )
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<LineHeightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    LineHeightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        // Computed value: for <length-percentage> the absolute length (percentages against
        //                 the element's own font size), otherwise as specified.
        c.inline_.cow().lineHeight = _value.visit(
            [](Keywords::Normal const& normal) -> LineHeight {
                return normal;
            },
            [](Css::Number const& number) -> LineHeight {
                return toComputed(number);
            },
            [&](Css::LengthPercentage const& length) -> LineHeight {
                return toComputed(length, cx, cx.fontSize);
            }
        );
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
export struct DominantBaselineProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::DOMINANT_BASELINE) {}

        Rc<Property> initial() const override {
            return makeRc<DominantBaselineProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<DominantBaselineProperty>(self(), c.inline_->dominantBaseline);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<DominantBaselineProperty>(self(), try$(parseValue<Css::DominantBaseline>(c))));
        }
    };

    Css::DominantBaseline _value;

    DominantBaselineProperty(Rc<Property::Registration> registration, Css::DominantBaseline value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inline_.cow().dominantBaseline = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-inline-3/#baseline-source
export struct BaselineSourceProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BASELINE_SOURCE) {}

        Rc<Property> initial() const override {
            return makeRc<BaselineSourceProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BaselineSourceProperty>(self(), c.inline_->baselineSource);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BaselineSourceProperty>(self(), try$(parseValue<Css::BaselineSource>(c))));
        }
    };

    Css::BaselineSource _value;

    BaselineSourceProperty(Rc<Property::Registration> registration, Css::BaselineSource value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inline_.cow().baselineSource = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-inline-3/#alignment-baseline-property
export struct AlignmentBaselineProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::ALIGNMENT_BASELINE) {}

        Rc<Property> initial() const override {
            return makeRc<AlignmentBaselineProperty>(self(), Keywords::BASELINE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<AlignmentBaselineProperty>(self(), c.inline_->alignmentBaseline);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignmentBaselineProperty>(self(), try$(parseValue<Css::AlignmentBaseline>(c))));
        }
    };

    Css::AlignmentBaseline _value;

    AlignmentBaselineProperty(Rc<Property::Registration> registration, Css::AlignmentBaseline value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inline_.cow().alignmentBaseline = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-inline/#propdef-baseline-shift
export struct BaselineShiftProperty : Property {
    using Value = Union<
        Css::LengthPercentage,
        Keywords::Sub,
        Keywords::Super,
        Keywords::Top,
        Keywords::Center,
        Keywords::Bottom>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BASELINE_SHIFT) {}

        Rc<Property> initial() const override {
            return makeRc<BaselineShiftProperty>(self(), Css::LengthPercentage::fromCanonical(0));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BaselineShiftProperty>(self(), toSpecified(c.inline_->baselineShift));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BaselineShiftProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BaselineShiftProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inline_.cow().baselineShift = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-inline/#transverse-alignment
export struct VerticalAlignProperty : Property {
    struct Value {
        Opt<Css::BaselineSource> baselineSource = NONE;
        Opt<Css::AlignmentBaseline> alignmentBaseline = NONE;
        Opt<BaselineShiftProperty::Value> baselineShift = NONE;
    };

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::VERTICAL_ALIGN, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<VerticalAlignProperty>(self(), Value{.alignmentBaseline = Some(Keywords::BASELINE)});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<VerticalAlignProperty>(
                self(),
                Value{
                    Some(c.inline_->baselineSource),
                    Some(c.inline_->alignmentBaseline),
                    Some(toSpecified(c.inline_->baselineShift)),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            auto value = Value{};

            while (not c.ended()) {
                auto maybeFirstOrLast = parseValue<Union<Keywords::First, Keywords::Last>>(c);
                if (maybeFirstOrLast) {
                    if (not value.baselineSource) {
                        value.baselineSource = Some(maybeFirstOrLast.expect().visit(
                            [](auto& v) -> Css::BaselineSource {
                                return v;
                            }
                        ));
                        continue;
                    }

                    return Error::invalidData("duplicated value type");
                }

                auto maybeAlignmentBaseline = parseValue<Css::AlignmentBaseline>(c);
                if (maybeAlignmentBaseline) {
                    if (not value.alignmentBaseline) {
                        value.alignmentBaseline = Some(maybeAlignmentBaseline.expect());
                        continue;
                    }

                    return Error::invalidData("duplicated value type");
                }

                auto maybeBaselineShift = parseValue<BaselineShiftProperty::Value>(c);
                if (maybeBaselineShift) {
                    if (not value.baselineShift) {
                        value.baselineShift = Some(maybeBaselineShift.expect());
                        continue;
                    }

                    return Error::invalidData("duplicated value type");
                }

                return Error::invalidData("unknown value");
            }

            if (not value.alignmentBaseline and not value.baselineSource and not value.baselineShift) {
                return Error::invalidData("missing value");
            }

            return Ok(makeRc<VerticalAlignProperty>(self(), value));
        }
    };

    Value _value;

    VerticalAlignProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        Longhands shorthands = {};

        if (auto const& [baselineSource] = _value.baselineSource) {
            shorthands.pushBack(
                makeRc<BaselineSourceProperty>(
                    registry.resolveRegistration(Properties::BASELINE_SOURCE, {}).take(),
                    baselineSource
                )
            );
        }

        if (auto const& [alignmentBaseline] = _value.alignmentBaseline) {
            shorthands.pushBack(
                makeRc<AlignmentBaselineProperty>(
                    registry.resolveRegistration(Properties::ALIGNMENT_BASELINE, {}).take(),
                    alignmentBaseline
                )
            );
        }

        if (auto const& [baselineShift] = _value.baselineShift) {
            shorthands.pushBack(
                makeRc<BaselineShiftProperty>(
                    registry.resolveRegistration(Properties::BASELINE_SHIFT, {}).take(),
                    baselineShift
                )
            );
        }

        return shorthands;
    }

    void repr(Io::Emit& e) const override {
        e("baseline-source={} alignment-baseline={}", _value.baselineSource, _value.alignmentBaseline);
    }
};

} // namespace Vaev::Style
