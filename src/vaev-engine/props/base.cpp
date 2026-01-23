module;

#include <karm/macros>

export module Vaev.Engine:props.base;

import Karm.Core;
import Karm.Logger;
import Karm.Debug;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

static auto debugProperties = Debug::Flag::debug("css-properties", "Print debug information about CSS properties");

namespace Properties {

#define PROPERTY(NAME, VALUE) export Symbol NAME = Symbol::from(VALUE);
#include "defs/properties.inc"
#undef PROPERTY

} // namespace Properties

export struct PropertyRegistry;

export struct Property : Meta::NoCopy {
    enum struct Options : u8 {
        // https://drafts.csswg.org/css-cascade-5/#inherited-value
        INHERITED = 1 << 0,

        // https://drafts.csswg.org/css-cascade-5/#shorthand
        SHORTHAND_PROPERTY = 1 << 1,

        // https://drafts.csswg.org/css-variables-2/#custom-property
        CUSTOM_PROPERTY = 1 << 2,

        // https://svgwg.org/svg2-draft/styling.html#PresentationAttributes
        PRESENTATION_ATTRIBUTE = 1 << 3,

        // Represent a miss-parsed property
        BOGUS_REGISTRATION = 1 << 4,

        // The property is not reset by all:
        // https://drafts.csswg.org/css-cascade/#all-shorthand
        ALL_EXCLUDED = 1 << 5
    };

    using enum Options;

    // https://drafts.css-houdini.org/css-properties-values-api/#custom-property-registration
    struct Registration : Meta::NoCopy {
        Opt<Weak<Registration>> _self;

        Rc<Registration> self() const {
            return _self
                .unwrap("node not self bound")
                .upgrade()
                .unwrap();
        }

        virtual ~Registration() = default;

        virtual Symbol name() const = 0;

        // https://drafts.csswg.org/css-cascade-4/#legacy-name-alias
        virtual Vec<Symbol> legacyAlias() const {
            return {};
        }

        virtual Flags<Options> flags() const {
            return {};
        }

        virtual Rc<Property> initial() const = 0;

        virtual Rc<Property> load(SpecifiedValues const& c) const = 0;

        virtual void inherit(SpecifiedValues const& parent, SpecifiedValues& child) {
            // NOTE: This is the slow fallback path for properties that are not
            //       commonly inherited. Any property marked with the INHERITED
            //       flag should override this method with a faster implementation.
            if (flags().has(INHERITED))
                logFatal("property {#} marked as INHERITED is using the slow fallback path. override inherit()!", name());
            load(parent)->apply(child);
        }

        virtual Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const = 0;

        // https://svgwg.org/svg2-draft/styling.html#PresentationAttributes
        virtual Res<Rc<Property>> parsePresentationAttribute(Str style) {
            Css::Lexer lex{style};
            Diag::Collector diags = Diag::Collector::ignore();
            auto [content, _] = Css::consumeDeclarationValue(lex, diags);
            Cursor<Css::Sst> cursor = content;
            return parse(cursor);
        }
    };

    Rc<Registration> registration;
    Css::Important important = Css::Important::UNSET;

    Property(Rc<Registration> registration)
        : registration(registration) {}

    virtual ~Property() = default;

    virtual Vec<Rc<Property>> expandShorthand(PropertyRegistry&, [[maybe_unused]] SpecifiedValues const& parent, [[maybe_unused]] SpecifiedValues& child) const {
        if (isBogusProperty())
            logFatal("trying to expand {#} as a bug property");

        if (isShorthandProperty())
            logFatal("shorthand property {#} is missing expandShorthand() implementation", registration->name());

        logFatal("expandShorthand() called on non shorthand property {#}", registration->name());
        return {};
    }

    virtual void apply(SpecifiedValues&) const {
        logFatal("longhand property {#} is missing apply() implementation", registration->name());
    }

    virtual void apply(SpecifiedValues const& parent, SpecifiedValues& child) const {
        (void)parent;
        apply(child);
    }

    virtual void repr(Io::Emit& e) const = 0;

    bool isCustomProperty() const {
        return registration->flags().has(CUSTOM_PROPERTY);
    }

    bool isShorthandProperty() const {
        return registration->flags().has(SHORTHAND_PROPERTY);
    }

    virtual bool isDefaulted() const {
        return false;
    }

    virtual bool isBogusProperty() const {
        return registration->flags().has(BOGUS_REGISTRATION);
    }
};

// MARK: Custom Property -------------------------------------------------------

// https://drafts.csswg.org/css-variables/#defining-variables
// this symbolizes a custom property, it starts with `--` and can be used to store a value that can be reused in the stylesheet
struct CustomProperty : Property {
    struct Registration : Property::Registration {
        Symbol _name;

        Registration(Symbol name) : _name(name) {
        }

        Symbol name() const override {
            return _name;
        }

        Flags<Options> flags() const override {
            return {INHERITED, CUSTOM_PROPERTY, ALL_EXCLUDED};
        }

        Rc<Property> initial() const override {
            return makeRc<CustomProperty>(
                self(), Css::Content{Css::Sst::guaranteedInvalid()}
            );
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<CustomProperty>(self(), c.getCustomProp(_name));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Css::Content content{c};
            c.next(c.rem());
            return Ok(makeRc<CustomProperty>(self(), std::move(content)));
        };
    };

    Css::Content _value;

    CustomProperty(Rc<Property::Registration> registration, Css::Content value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& child) const override {
        child.setCustomProp(registration->name(), _value);
    }

    void repr(Io::Emit& e) const override {
        e("(custom {})", _value);
    }
};

// MARK: Deferred Property ------------------------------------------------------

// NOTE: A property that could not be parsed, it's used to store the value
//       as-is and apply it with the cascade and custom properties
struct DeferredProperty : Property {
    Css::Content _value;

    DeferredProperty(Rc<Property::Registration> registration, Css::Content value)
        : Property(registration), _value(value) {}

    static bool _expandVariable(Cursor<Css::Sst>& c, Map<Symbol, Css::Content> const& env, Css::Content& out) {
        if (not(c->type == Css::Sst::FUNC and
                c->prefix == Css::Token::function("var("))) {
            return false;
        }

        Cursor<Css::Sst> content = c->content;

        eatWhitespace(content);
        if (content.ended())
            return true;

        if (content.peek() != Css::Token::IDENT)
            return true;

        Symbol varName = Symbol::from(content->token.data);
        if (auto ref = env.access(varName)) {
            Cursor<Css::Sst> varContent = *ref;
            _expandContent(varContent, env, out);
            return true;
        }
        content.next();

        eatWhitespace(content);

        if (not content.skip(Css::Token::COMMA))
            return true;

        _expandContent(content, env, out);
        return true;
    }

    static bool _expandFunction(Cursor<Css::Sst>& c, Map<Symbol, Css::Content> const& env, Css::Content& out) {
        if (c->type != Css::Sst::FUNC)
            return false;

        auto& func = out.emplaceBack(Css::Sst::FUNC);
        func.prefix = c->prefix;
        Cursor<Css::Sst> content = c->content;
        _expandContent(content, env, func.content);

        return true;
    }

    static void _expandContent(Cursor<Css::Sst>& c, Map<Symbol, Css::Content> const& env, Css::Content& out) {
        // NOTE: Hint that we will add all the remaining elements
        out.ensure(out.len() + c.rem());

        while (not c.ended()) {
            if (not _expandVariable(c, env, out) and
                not _expandFunction(c, env, out)) {
                out.pushBack(*c);
            }

            c.next();
        }
    }

    Res<Rc<Property>> _expandProperty(SpecifiedValues& child) const {
        Cursor<Css::Sst> cursor = _value;
        Css::Content out;
        _expandContent(cursor, *child.variables, out);
        cursor = out;

        // Expanding the variable might have introduced some leading whitespace
        // Accordingly, we need to eat them before parsing
        // See: https://www.w3.org/TR/css-syntax-3/#parse-declaration
        eatWhitespace(cursor);

        auto prop = registration->parse(cursor);
        if (not prop and debugProperties) {
            logWarn("failed to parse declaration: {}: {}", registration->name(), prop);
            logInfo("Here: {}", out);
        }
        return prop;
    }

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const& parent, SpecifiedValues& child) const override {
        if (not isShorthandProperty())
            logFatal("expandShorthand called on non shorthand property {#}", registration->name());

        auto prop = _expandProperty(child);
        if (not prop)
            return {};
        return prop.unwrap()->expandShorthand(registry, parent, child);
    }

    void apply(SpecifiedValues const& parent, SpecifiedValues& child) const override {
        auto prop = _expandProperty(child);
        if (not prop)
            return;
        prop.unwrap()->apply(parent, child);
    }

    void repr(Io::Emit& e) const override {
        e("(deferred {})", _value);
    }
};

// MARK: Defaulted Property ----------------------------------------------------

enum struct Default {
    INITIAL, //< represents the value defined as the property’s initial value.
    INHERIT, //< represents the property’s computed value on the parent element.
    UNSET,   //< acts as either inherit or initial, depending on whether the property is inherited or not.
    REVERT,  //< rolls back the cascade to the cascaded value of the earlier origin.

    _LEN,
};

struct DefaultedProperty : Property {
    Default _value;

    DefaultedProperty(Rc<Property::Registration> registration, Default value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const& parent, SpecifiedValues& child) const override {
        if (_value == Default::INITIAL) {
            // The initial CSS-wide keyword represents the value
            // defined as the property’s initial value.
            // https://drafts.csswg.org/css-cascade/#initial
            return registration->initial()->expandShorthand(registry, parent, child);
        } else if (_value == Default::INHERIT) {
            // The inherit CSS-wide keyword represents the property’s
            // computed value on the parent element.
            // https://drafts.csswg.org/css-cascade/#inherit
            return registration->load(parent)->expandShorthand(registry, parent, child);
        } else if (_value == Default::UNSET) {
            // The unset CSS-wide keyword acts as either inherit or initial,
            // depending on whether the property is inherited or not.
            // https://drafts.csswg.org/css-cascade/#inherit-initial
            if (registration->flags().has(INHERITED))
                return registration->load(parent)->expandShorthand(registry, parent, child);

            return registration->initial()->expandShorthand(registry, parent, child);
        } else {
            unreachable();
        }
    }

    void apply(SpecifiedValues const& parent, SpecifiedValues& child) const override {
        if (_value == Default::INITIAL) {
            // The initial CSS-wide keyword represents the value
            // defined as the property’s initial value.
            // https://drafts.csswg.org/css-cascade/#initial
            registration->initial()->apply(parent, child);
        } else if (_value == Default::INHERIT) {
            // The inherit CSS-wide keyword represents the property’s
            // computed value on the parent element.
            // https://drafts.csswg.org/css-cascade/#inherit
            registration->load(parent)->apply(child);
        } else if (_value == Default::UNSET) {
            // The unset CSS-wide keyword acts as either inherit or initial,
            // depending on whether the property is inherited or not.
            // https://drafts.csswg.org/css-cascade/#inherit-initial
            if (registration->flags().has(INHERITED))
                registration->load(parent)->apply(child);
            else
                registration->initial()->apply(parent, child);

        } else {
            unreachable();
        }
    }

    bool isDefaulted() const override {
        return true;
    }

    void repr(Io::Emit& e) const override {
        e("(defaulted {})", _value);
    }
};

// MARK: Bogus Property --------------------------------------------------------

// Represent a property that could not be parsed
struct BogusProperty : Property {
    struct Registration : Property::Registration {
        Symbol _name;

        Registration(Symbol name)
            : _name(name) {}

        Flags<Options> flags() const override {
            return {BOGUS_REGISTRATION};
        }

        Symbol name() const override {
            return _name;
        }

        Rc<Property> initial() const override {
            unreachable();
        }

        Rc<Property> load(SpecifiedValues const&) const override {
            unreachable();
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>&) const override {
            unreachable();
        }
    };

    Css::Content _value;
    Error _error;

    BogusProperty(Rc<Property::Registration> registration, Css::Content value, Error error)
        : Property(registration),
          _value(value),
          _error(error) {}

    bool isBogusProperty() const override {
        return true;
    }

    void repr(Io::Emit& e) const override {
        e("{} (error:{})", _value, _error);
    }
};

// MARK: Property Registry -----------------------------------------------------

export struct PropertyRegistry {
    Map<Symbol, Symbol> _legacyAlias;
    Map<Symbol, Rc<Property::Registration>> _registrations;
    Map<Symbol, Rc<Property::Registration>> _presentationAttributes;

    enum struct Options : u8 {
        GENERATE_BOGUS = 1 << 0,
        GENERATE_CUSTOM_PROPERTY = 1 << 1,
        DEFER_UNPARSABLE = 1 << 2,
        ALLOW_DEFAULTING = 1 << 3,

        TOP_LEVEL = GENERATE_BOGUS | GENERATE_CUSTOM_PROPERTY | DEFER_UNPARSABLE | ALLOW_DEFAULTING,
    };

    using enum Options;

    auto const& registrations() const {
        return _registrations;
    }

    void registerProperty(Symbol propertyName, Rc<Property::Registration> registration) {
        if (registration->flags().has(Property::PRESENTATION_ATTRIBUTE))
            _presentationAttributes.put(propertyName, registration);

        for (auto legacyAlias : registration->legacyAlias())
            _legacyAlias.put(legacyAlias, registration->name());

        _registrations.put(propertyName, registration);
    }

    template <typename Property>
    void registerProperty() {
        auto registration = makeRc<typename Property::Registration>();
        registration->_self = registration;
        registerProperty(registration->name(), registration);
    }

    Rc<Property::Registration> registerCustomProperty(Symbol propertyName) {
        auto registration = makeRc<CustomProperty::Registration>(propertyName);
        registration->_self = registration;
        registerProperty(propertyName, registration);
        return registration;
    }

    Opt<Rc<Property::Registration>> resolveRegistration(Symbol propertyName, Flags<Options> options) {
        propertyName = _legacyAlias.tryGet(propertyName).unwrapOr(propertyName);

        if (auto maybeRegistration = _registrations.tryGet(propertyName))
            return maybeRegistration.take();

        if (options.has(GENERATE_CUSTOM_PROPERTY)) {
            if (startWith(propertyName.str(), "--"s) != Match::NO)
                return registerCustomProperty(propertyName);
        }

        if (options.has(GENERATE_BOGUS)) {
            return makeRc<BogusProperty::Registration>(propertyName);
        }

        return NONE;
    }

    Opt<Rc<Property::Registration>> resolveRegistration(Str propertyName, Flags<Options> options) {
        return resolveRegistration(Symbol::from(propertyName), options);
    }

    // MARK: Value Parsing -----------------------------------------------------

    Res<Rc<Property>> _parseDefaulted(Rc<Property::Registration> registration, Cursor<Css::Sst>& content) {
        Default value;
        if (content.skip(Css::Token::ident("initial"))) {
            value = Default::INITIAL;
        } else if (content.skip(Css::Token::ident("inherit"))) {
            value = Default::INHERIT;
        } else if (content.skip(Css::Token::ident("unset"))) {
            value = Default::UNSET;
        } else if (content.skip(Css::Token::ident("revert"))) {
            value = Default::REVERT;
        } else {
            return Error::invalidData("unknown declaration");
        }

        return Ok(makeRc<DefaultedProperty>(registration, value));
    }

    Rc<Property> _deferProperty(Rc<Property::Registration> registration, Slice<Css::Sst> content) {
        return makeRc<DeferredProperty>(registration, content);
    }

    Res<Rc<Property>> parseValue(Symbol propertyName, Slice<Css::Sst> content, Flags<Options> options) {
        Cursor cursor = content;

        auto registration = try$(resolveRegistration(propertyName, options));
        if (registration->flags().has(Property::BOGUS_REGISTRATION))
            return Ok(makeRc<BogusProperty>(registration, content, Error::invalidData("unknow property")));

        eatWhitespace(cursor);
        if (options.has(ALLOW_DEFAULTING)) {
            if (auto defaulted = _parseDefaulted(registration, cursor))
                return Ok(defaulted.take());
        }

        auto maybeProp = registration->parse(cursor);

        if (options.has(DEFER_UNPARSABLE)) {
            if (not maybeProp)
                return Ok(_deferProperty(registration, content));
        }

        eatWhitespace(cursor);

        if (options.has(GENERATE_BOGUS)) {
            if (not cursor.ended()) {
                auto registration = makeRc<BogusProperty::Registration>(propertyName);
                return Ok(makeRc<BogusProperty>(registration, content, Error::invalidData("un-consumed token in property value")));
            }
        }

        return maybeProp;
    }

    Res<Rc<Property>> parseValue(Symbol propertyName, Str propertyValue, Flags<Options> options) {
        Css::Lexer lex{propertyValue};
        Diag::Collector diags = Diag::Collector::ignore();
        auto [content, _] = Css::consumeDeclarationValue(lex, diags);
        return parseValue(propertyName, content, options);
    }

    // MARK: Declaration Parsing -----------------------------------------------

    Res<Rc<Property>> parseDeclaration(Css::Sst const& sst, Flags<Options> options) {
        if (sst != Css::Sst::DECL)
            panic("expected declaration");

        if (sst.token != Css::Token::IDENT)
            panic("expected ident");

        auto propertyName = Symbol::from(sst.token.data);
        Cursor<Css::Sst> content = sst.content;
        auto prop = try$(parseValue(propertyName, content, options));
        prop->important = sst.important;
        return Ok(prop);
    }

    Vec<Rc<Property>> parseDeclarations(Css::Content const& sst, Flags<Options> options) {
        Vec<Rc<Property>> res;

        for (auto const& item : sst) {
            if (item != Css::Sst::DECL)
                continue;

            auto prop = parseDeclaration(item, options);
            if (not prop)
                continue;
            res.pushBack(prop.take());
        }

        return res;
    }

    Vec<Rc<Property>> parseDeclarations(Css::Sst const& sst, Flags<Options> options) {
        return parseDeclarations(sst.content, options);
    }

    Vec<Rc<Property>> parseDeclarations(Str style, Flags<Options> options) {
        Css::Lexer lex{style};
        auto diags = Diag::Collector::ignore();
        auto sst = Css::consumeDeclarationList(lex, diags, true);
        return parseDeclarations(sst, options);
    }

    Opt<Rc<Property>> parsePresentationAttribute(Symbol attrName, Str value) const {
        Symbol lower = Symbol::from(toLower(attrName.str()));
        auto registration = try$(_presentationAttributes.tryGet(lower));
        return registration->parsePresentationAttribute(value).ok();
    }
};

} // namespace Vaev::Style
