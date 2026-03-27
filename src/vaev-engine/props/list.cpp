module;

#include <karm/macros>

export module Vaev.Engine:props.list;

import :props.base;

namespace Vaev::Style {

// https://www.w3.org/TR/css-lists-3/#propdef-list-style-image
struct ListStyleImageProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::LIST_STYLE_IMAGE;
        }

        Flags<Options> flags() const override {
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<ListStyleImageProperty>(self(), Keywords::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ListStyleImageProperty>(self(), c.list->image);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto value = try$(parseValue<ListImage>(c));
            return Ok(makeRc<ListStyleImageProperty>(self(), value));
        }
    };

    ListImage _value;

    ListStyleImageProperty(Rc<Property::Registration> registration, ListImage value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.list.cow().image = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-lists-3/#propdef-list-style-type
struct ListStyleTypeProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::LIST_STYLE_TYPE;
        }

        Flags<Options> flags() const override {
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<ListStyleTypeProperty>(self(), CustomIdent{"disc"_sym});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ListStyleTypeProperty>(self(), c.list->type);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto value = try$(parseValue<ListType>(c));
            return Ok(makeRc<ListStyleTypeProperty>(self(), value));
        }
    };

    ListType _value;

    ListStyleTypeProperty(Rc<Property::Registration> registration, ListType value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.list.cow().type = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-lists-3/#list-style-position-property
struct ListStylePositionProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::LIST_STYLE_POSITION;
        }

        Flags<Options> flags() const override {
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<ListStylePositionProperty>(self(), Keywords::OUTSIDE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ListStylePositionProperty>(self(), c.list->position);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto value = try$(parseValue<ListPosition>(c));
            return Ok(makeRc<ListStylePositionProperty>(self(), value));
        }
    };

    ListPosition _value;

    ListStylePositionProperty(Rc<Property::Registration> registration, ListPosition value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.list.cow().position = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-lists-3/#propdef-list-style
struct ListStyleProperty : Property {
    struct Value {
        ListImage image = Keywords::NONE;
        ListType type = CustomIdent{"disc"_sym};
        ListPosition position = Keywords::OUTSIDE;

        void repr(Io::Emit& e) const {
            e("(list-style {} {} {})", image, type, position);
        }
    };

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::LIST_STYLE;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<ListStyleProperty>(self(), Value{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ListStyleProperty>(
                self(),
                Value{
                    c.list->image,
                    c.list->type,
                    c.list->position,
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Value value;
            while (not c.ended()) {
                auto image = parseValue<ListImage>(c);
                if (image) {
                    value.image = image.take();
                    continue;
                }

                auto type = parseValue<ListType>(c);
                if (type) {
                    value.type = type.take();
                    continue;
                }

                auto position = parseValue<ListPosition>(c);
                if (position) {
                    value.position = position.take();
                    continue;
                }

                return Error::invalidData("unexpected token");
            }
            return Ok(makeRc<ListStyleProperty>(self(), std::move(value)));
        }
    };

    Value _value;

    ListStyleProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
        return {
            makeRc<ListStyleImageProperty>(registry.resolveRegistration(Properties::LIST_STYLE_IMAGE, {}).unwrap(), _value.image),
            makeRc<ListStyleTypeProperty>(registry.resolveRegistration(Properties::LIST_STYLE_TYPE, {}).unwrap(), _value.type),
            makeRc<ListStylePositionProperty>(registry.resolveRegistration(Properties::LIST_STYLE_POSITION, {}).unwrap(), _value.position),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-lists-3/#propdef-marker-side
struct MarkerSideProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARKER_SIDE;
        }

        Rc<Property> initial() const override {
            return makeRc<MarkerSideProperty>(self(), Keywords::MATCH_SELF);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarkerSideProperty>(self(), c.list->markerSide);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto value = try$(parseValue<MarkerSide>(c));
            return Ok(makeRc<MarkerSideProperty>(self(), value));
        }
    };

    MarkerSide _value;

    MarkerSideProperty(Rc<Property::Registration> registration, MarkerSide value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.list.cow().markerSide = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
