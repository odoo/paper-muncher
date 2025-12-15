module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.baseline;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

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

        Rc<Property> load(SpecifiedValues const&) const override {
            return makeRc<LineHeightProperty>(self(), LineHeight::NORMAL); // TODO
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<LineHeightProperty>(self(), try$(parseValue<LineHeight>(c))));
        }
    };

    LineHeight _value;

    LineHeightProperty(Rc<Property::Registration> registration, LineHeight value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues&) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<DominantBaselineProperty>(self(), c.baseline->dominant);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<DominantBaselineProperty>(self(), try$(parseValue<DominantBaseline>(c))));
        }
    };

    DominantBaseline _value;

    DominantBaselineProperty(Rc<Property::Registration> registration, DominantBaseline value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BaselineSourceProperty>(self(), c.baseline->source);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BaselineSourceProperty>(self(), try$(parseValue<BaselineSource>(c))));
        }
    };

    BaselineSource _value;

    BaselineSourceProperty(Rc<Property::Registration> registration, BaselineSource value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<AlignmentBaselineProperty>(self(), c.baseline->alignment);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignmentBaselineProperty>(self(), try$(parseValue<AlignmentBaseline>(c))));
        }
    };

    AlignmentBaseline _value;

    AlignmentBaselineProperty(Rc<Property::Registration> registration, AlignmentBaseline value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.baseline.cow().alignment = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
