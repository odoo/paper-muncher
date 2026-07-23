module;

#include <karm/macros>

export module Vaev.Engine:props.baseline;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-inline/#line-height-property
export struct LineHeightProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::LINE_HEIGHT;
        }

        Flags<Options> flags() const override {
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<LineHeightProperty>(self(), LineHeight::NORMAL);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            // TODO
            (void)parent;
            (void)child;
        }

        Rc<Property> load(ComputedValues const&) const override {
            return makeRc<LineHeightProperty>(self(), LineHeight::NORMAL); // TODO
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<LineHeightProperty>(self(), try$(parseValue<LineHeight>(c))));
        }
    };

    LineHeight _value;

    LineHeightProperty(Rc<Property::Registration> registration, LineHeight value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues const&, ComputedValues&, [[maybe_unused]] ComputationContext const& cx) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
export struct DominantBaselineProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::DOMINANT_BASELINE;
        }

        Rc<Property> initial() const override {
            return makeRc<DominantBaselineProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<DominantBaselineProperty>(self(), c.baseline->dominant);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<DominantBaselineProperty>(self(), try$(parseValue<DominantBaseline>(c))));
        }
    };

    DominantBaseline _value;

    DominantBaselineProperty(Rc<Property::Registration> registration, DominantBaseline value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.baseline.cow().dominant = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-inline-3/#baseline-source
export struct BaselineSourceProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BASELINE_SOURCE;
        }

        Rc<Property> initial() const override {
            return makeRc<BaselineSourceProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BaselineSourceProperty>(self(), c.baseline->source);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BaselineSourceProperty>(self(), try$(parseValue<BaselineSource>(c))));
        }
    };

    BaselineSource _value;

    BaselineSourceProperty(Rc<Property::Registration> registration, BaselineSource value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.baseline.cow().source = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-inline-3/#alignment-baseline-property
export struct AlignmentBaselineProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::ALIGNMENT_BASELINE;
        }

        Rc<Property> initial() const override {
            return makeRc<AlignmentBaselineProperty>(self(), Keywords::BASELINE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<AlignmentBaselineProperty>(self(), c.baseline->alignment);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignmentBaselineProperty>(self(), try$(parseValue<AlignmentBaseline>(c))));
        }
    };

    AlignmentBaseline _value;

    AlignmentBaselineProperty(Rc<Property::Registration> registration, AlignmentBaseline value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.baseline.cow().alignment = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-inline/#propdef-baseline-shift
export struct BaselineShiftProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BASELINE_SHIFT;
        }

        Rc<Property> initial() const override {
            return makeRc<BaselineShiftProperty>(self(), CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BaselineShiftProperty>(self(), c.baseline->shift);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BaselineShiftProperty>(self(), try$(parseValue<BaselineShift>(c))));
        }
    };

    BaselineShift _value;

    BaselineShiftProperty(Rc<Property::Registration> registration, BaselineShift value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.baseline.cow().shift = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-inline/#transverse-alignment
export struct VerticalAlignProperty : Property {
    struct Value {
        Opt<BaselineSource> baselineSource = NONE;
        Opt<AlignmentBaseline> alignmentBaseline = NONE;
        Opt<BaselineShift> baselineShift = NONE;
    };

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::VERTICAL_ALIGN;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<VerticalAlignProperty>(self(), Value{.alignmentBaseline = Keywords::BASELINE});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<VerticalAlignProperty>(
                self(),
                Value{
                    c.baseline->source,
                    c.baseline->alignment,
                    c.baseline->shift,
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
                        value.baselineSource = maybeFirstOrLast.unwrap().visit(
                            [](auto& v) -> BaselineSource {
                                return v;
                            }
                        );
                        continue;
                    }

                    return Error::invalidData("duplicated value type");
                }

                auto maybeAlignmentBaseline = parseValue<AlignmentBaseline>(c);
                if (maybeAlignmentBaseline) {
                    if (not value.alignmentBaseline) {
                        value.alignmentBaseline = maybeAlignmentBaseline.unwrap();
                        continue;
                    }

                    return Error::invalidData("duplicated value type");
                }

                auto maybeBaselineShift = parseValue<BaselineShift>(c);
                if (maybeBaselineShift) {
                    if (not value.baselineShift) {
                        value.baselineShift = maybeBaselineShift.unwrap();
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

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        Vec<Rc<Property>> shorthands = {};

        if (auto [baselineSource] = _value.baselineSource) {
            shorthands.pushBack(
                makeRc<BaselineSourceProperty>(
                    registry.resolveRegistration(Properties::BASELINE_SOURCE, {}).take(),
                    baselineSource
                )
            );
        }

        if (auto [alignmentBaseline] = _value.alignmentBaseline) {
            shorthands.pushBack(
                makeRc<AlignmentBaselineProperty>(
                    registry.resolveRegistration(Properties::ALIGNMENT_BASELINE, {}).take(),
                    alignmentBaseline
                )
            );
        }

        if (auto [baselineShift] = _value.baselineShift) {
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
