module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.sizing;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// MARK: Sizing ----------------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export struct BoxSizingProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BOX_SIZING;
        }

        Rc<Property> initial() const override {
            return makeRc<BoxSizingProperty>(self(), BoxSizing::CONTENT_BOX);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BoxSizingProperty>(self(), c.boxSizing);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            BoxSizing value;
            if (c.skip(Css::Token::ident("border-box")))
                value = BoxSizing::BORDER_BOX;
            else if (c.skip(Css::Token::ident("content-box")))
                value = BoxSizing::CONTENT_BOX;
            else
                return Error::invalidData("expected 'border-box' or 'content-box'");

            return Ok(makeRc<BoxSizingProperty>(self(), value));
        }
    };

    BoxSizing _value;

    BoxSizingProperty(Rc<Property::Registration> registration, BoxSizing value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.boxSizing = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width
export struct WidthProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::WIDTH;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<WidthProperty>(self(), Size{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<WidthProperty>(self(), c.sizing->width);
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            if (auto prop = Property::Registration::parsePresentationAttribute(style))
                return prop;
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<WidthProperty>(self(), try$(parseValue<Size>(c))));
        }
    };

    Size _value;

    WidthProperty(Rc<Property::Registration> registration, Size value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.sizing.cow().width = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-height

export struct HeightProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::HEIGHT;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<HeightProperty>(self(), Size{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<HeightProperty>(self(), c.sizing->height);
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            if (auto prop = Property::Registration::parsePresentationAttribute(style))
                return prop;
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<HeightProperty>(self(), try$(parseValue<Size>(c))));
        }
    };

    Size _value;

    HeightProperty(Rc<Property::Registration> registration, Size value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.sizing.cow().height = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-width
export struct MinWidthProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MIN_WIDTH;
        }

        Rc<Property> initial() const override {
            return makeRc<MinWidthProperty>(self(), Size{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MinWidthProperty>(self(), c.sizing->minWidth);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MinWidthProperty>(self(), try$(parseValue<Size>(c))));
        }
    };

    Size _value;

    MinWidthProperty(Rc<Property::Registration> registration, Size value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.sizing.cow().minWidth = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-height
export struct MinHeightProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MIN_HEIGHT;
        }

        Rc<Property> initial() const override {
            return makeRc<MinHeightProperty>(self(), Size{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MinHeightProperty>(self(), c.sizing->minHeight);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MinHeightProperty>(self(), try$(parseValue<Size>(c))));
        }
    };

    Size _value;

    MinHeightProperty(Rc<Property::Registration> registration, Size value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.sizing.cow().minHeight = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-width

export struct MaxWidthProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MAX_WIDTH;
        }

        Rc<Property> initial() const override {
            return makeRc<MaxWidthProperty>(self(), MaxSize{Keywords::NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MaxWidthProperty>(self(), c.sizing->maxWidth);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MaxWidthProperty>(self(), try$(parseValue<MaxSize>(c))));
        }
    };

    MaxSize _value;

    MaxWidthProperty(Rc<Property::Registration> registration, MaxSize value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.sizing.cow().maxWidth = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-height
export struct MaxHeightProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MAX_HEIGHT;
        }

        Rc<Property> initial() const override {
            return makeRc<MaxHeightProperty>(self(), MaxSize{Keywords::NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MaxHeightProperty>(self(), c.sizing->maxHeight);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MaxHeightProperty>(self(), try$(parseValue<MaxSize>(c))));
        }
    };

    MaxSize _value;

    MaxHeightProperty(Rc<Property::Registration> registration, MaxSize value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.sizing.cow().maxHeight = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
