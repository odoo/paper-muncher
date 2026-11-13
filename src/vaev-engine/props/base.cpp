module;

#include <karm/macros>

export module Vaev.Engine:props.base;

import Karm.Core;
import Karm.Logger;
import Karm.Debug;
import :css.parser;
import :style.computed;
import :style.viewport;

using namespace Karm;

namespace Vaev::Style {

static auto debugProperties = Debug::Flag::debug("css-properties", "Print debug information about CSS properties");

namespace Properties {

#define LONGHAND(NAME, VALUE) export Symbol NAME = Symbol::from(VALUE);
#include "defs/longhands.inc"
#undef LONGHAND

#define SHORTHAND(NAME, VALUE) export Symbol NAME = Symbol::from(VALUE);
#include "defs/shorthands.inc"
#undef SHORTHAND

} // namespace Properties

export struct RegisteredPropertySet;

export struct ComputationContext :
    RelativeLengthContextData,
    FontSizeContextData {

    void populateUsingViewport(Viewport const& vp) {
        viewportSmallWidth = vp.small.width.cast<f64>();
        viewportSmallHeight = vp.small.height.cast<f64>();

        viewportLargeWidth = vp.large.width.cast<f64>();
        viewportLargeHeight = vp.large.height.cast<f64>();

        viewportDynamicWidth = vp.dynamic.width.cast<f64>();
        viewportDynamicHeight = vp.dynamic.height.cast<f64>();
    }

    void populateUsingRootComputedValues(ComputedValues const& values) {
        Gfx::Font font = Gfx::Font{
            values.fontFace,
            values.font->size.cast<f64>(),
        };
        rootFontSize = font.fontSize();
        rootXHeight = font.xHeight();
        rootCapHeight = font.capHeight();
        rootZeroAdvance = font.zeroAdvance();
        rootLineHeight = font.lineHeight();
    }

    void populateUsingParentComputedValues(ComputedValues const& values) {
        parentFontSize = values.font->size.cast<f64>();
    }

    void populateUsingOwnComputedValues(ComputedValues const& values) {
        Gfx::Font font = Gfx::Font{
            values.fontFace,
            values.font->size.cast<f64>(),
        };
        fontSize = font.fontSize();
        xHeight = font.xHeight();
        capHeight = font.capHeight();
        zeroAdvance = font.zeroAdvance();
        lineHeight = font.lineHeight();
    }
};
export struct Property;
export using Longhands = SmallVec<Rc<Property>, 12>;

/// Defines the metadata, lifecycle, and parsing rules for a specific CSS property.
///
/// The `Registration` acts as a singleton-like factory and behavior contract
/// for all instances of a given property. It handles parsing raw CSS tokens
/// into a concrete `Property` instance and dictates how the property interacts
/// with the cascade (inheritance, initial values).
// https://drafts.css-houdini.org/css-properties-values-api/#custom-property-registration
export struct Registration : Meta::NoCopy {
    Symbol name;

    Registration(Symbol name)
        : name(name) {}

    virtual ~Registration() = default;

    // https://drafts.csswg.org/css-cascade-4/#legacy-name-alias
    virtual SmallVec<Symbol, 1> legacyNameAlias() const {
        return {};
    }
};


export template <typename T>
Rc<typename T::Registration> const& registrationOf() {
    static auto registration = makeRc<typename T::Registration>();
    return registration;
}

enum struct ShorthandId {
#define SHORTHAND(ID, NAME) ID,
#include "defs/shorthands.inc"
#undef SHORTHAND
    _LEN,
};

enum struct LonghandId {
#define LONGHAND(ID, NAME) ID,
#include "defs/longhands.inc"
#undef LONGHAND
    CUSTOM,
    _LEN,
};

export static constexpr Array _shorthandNames = {
#define SHORTHAND(ID, NAME) NAME ""s,
#include "defs/shorthands.inc"
#undef SHORTHAND
};

export constexpr Str propertyName(ShorthandId id) {
    return _shorthandNames[toUnderlyingType(id)];
}

export static constexpr Array _longhandNames = {
#define LONGHAND(ID, NAME) NAME ""s,
#include "defs/longhands.inc"
#undef LONGHAND
};

export constexpr Str propertyName(LonghandId id) {
    return _longhandNames[toUnderlyingType(id)];
}

using PropertyId = Union<ShorthandId, LonghandId>;

/// How a property value is parsed, depending on where it comes from.
export enum struct ParsingMode : u8 {
    STANDARD,

    // https://quirks.spec.whatwg.org/
    QUIRKS,

    // https://svgwg.org/svg2-draft/styling.html#PresentationAttributes
    PRESENTATION_ATTRIBUTE,
};

// https://svgwg.org/svg2-draft/types.html#presentation-attribute-css-value
export template <typename T>
Res<T> parseValueWithUnitlessLength(Cursor<Css::Sst>& c, ParsingMode mode) {
    eatWhitespace(c);
    if (mode == ParsingMode::PRESENTATION_ATTRIBUTE and c.peek() == Css::Token::NUMBER) {
        Css::Sst length = c.peek();
        StringBuilder sb;
        sb.append(length.token.data.str());
        sb.append("px"s);
        length.token = {Css::Token::DIMENSION, sb.take()};

        Css::Content content;
        content.pushBack(std::move(length));
        Cursor<Css::Sst> lengthCursor = content;
        if (auto value = parseValue<T>(lengthCursor); value and lengthCursor.ended()) {
            c.next();
            return value;
        }
    }
    return parseValue<T>(c);
}

enum struct ComputationPhase {
    /// For properties which font-* properties depend on,
    /// they must not accept <length> as their value.
    PRE_FONT,

    /// For properties that are used to decide the font-face.
    /// If they accept <length>, it must be computed using the
    /// parent's font-face.
    FONT,

    /// For properties that can accept <length> as a value.
    /// Should be the default for most cases.
    NORMAL,

    /// For properties with a dependency on a property within the normal pass.
    LATE,
};

export struct ShorthandRegistration : Registration {
    using Registration::Registration;

    virtual Res<> parse(Cursor<Css::Sst>&, Vec<Rc<Property>>& stylesheet) const = 0;
    virtual Slice<Symbol> longhands() const = 0;
};

Opt<PropertyId> lookupProperty(Symbol name);
Rc<Property> initialProperty(LonghandId id);
Rc<Property> loadProperty(LonghandId id, ComputedValues const& c);
void inheritProperty(LonghandId id, ComputedValues const& parent, ComputedValues& child);
Res<Rc<Property>> parseProperty(LonghandId id, Cursor<Css::Sst>& c, ParsingMode mode = ParsingMode::STANDARD);

export struct Property : Meta::NoCopy {
    enum struct Options : u8 {
        // https://drafts.csswg.org/css-cascade-5/#inherited-value
        INHERITED = 1 << 0,

        // https://svgwg.org/svg2-draft/styling.html#PresentationAttributes
        PRESENTATION_ATTRIBUTE = 1 << 1,

        // The property is not reset by all:
        // https://drafts.csswg.org/css-cascade/#all-shorthand
        ALL_EXCLUDED = 1 << 2,

        /// The property participates in a bulk-inheritance optimization.
        ///
        /// Instead of calling the virtual `inherit()` method for every individual
        /// property, the engine performs a "fast-path" copy of large state blocks
        /// (e.g., Custom Properties) from parent to child.
        BULK_INHERITED = 1 << 3,
    };

    using enum Options;

    static constexpr Flags<Options> FLAGS = {};
    static constexpr ComputationPhase PHASE = ComputationPhase::NORMAL;
    static constexpr Slice<Str> LEGACY_NAME_ALIASES = {};

    LonghandId id;
    bool important = false;

    Property(LonghandId id)
        : id(id) {}

    virtual ~Property() = default;

    virtual void apply([[maybe_unused]] ComputedValues const& parent, [[maybe_unused]] ComputedValues& child, [[maybe_unused]] ComputationContext const& cx) const {
        logFatal("longhand property {:#} is missing apply() implementation", propertyName(id));
    }

    virtual void repr(Io::Emit& e) const = 0;

    virtual Res<> serialize(Io::TextWriter& w) const {
        auto e = Io::Emit{w};
        repr(e);
        return Ok();
    }

    /// Evaluates and returns the computed value of this property without permanently
    /// altering the target element's computed values.
    Rc<Property> computedValue(ComputedValues const& parent, ComputedValues& child) const {
        ComputationContext cx{};
        auto save = registration->load(child);
        apply(parent, child, cx);
        auto result = registration->load(child);
        save->apply(parent, child, cx);
        return result;
    }

    // bool isCustomProperty() const {
    //     return registration->flags.has(CUSTOM_PROPERTY);
    // }

    virtual bool isDefaulted() const {
        return false;
    }

    bool isInitial() const {
        return *initialProperty(id) == *this;
    }

    /// Determines whether this property instance represents a parsing failure or an unrecognized CSS declaration.
    virtual bool isBogusProperty() const {
        return false;
    }

    bool operator==(Property const& other) const {
        // HACK: This is a temporary implementation, it should be replaced
        //       with a more robust equality check that compares the actual
        //       values of the properties.
        return Io::toStr(*this) == Io::toStr(other);
    }
};

// MARK: Custom Property -------------------------------------------------------

// https://drafts.csswg.org/css-variables/#defining-variables
// this symbolizes a custom property, it starts with `--` and can be used to store a value that can be reused in the stylesheet
struct CustomProperty : Property {
    struct Value {
        Symbol name;
        Css::Content content;
    };

    Value _value;

    CustomProperty(Value const& value)
        : Property(LonghandId::CUSTOM), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.setCustomProp(_value.name, _value.content);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serialize(w, _value.content);
    }

    void repr(Io::Emit& e) const override {
        e("(custom {})", _value);
    }
};

// MARK: Toggle Property -------------------------------------------------------

// https://drafts.csswg.org/css-values-5/#toggle-notation
// The toggle() expression allows descendant elements to cycle over a list of values instead of inheriting the same value.
// struct ToggleProperty : Property {
//     Vec<Rc<Property>> _value;
//
//     ToggleProperty(Rc<LonghandRegistration> registration, Vec<Rc<Property>> value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const& parent, ComputedValues& child) const override {
//         // If toggle() is used on a shorthand property, it sets each of its
//         // longhands to a toggle() value with arguments corresponding to what
//         // the longhand would have received had each of the original toggle()
//         // arguments been the sole value of the shorthand.
//         Map<Symbol, Vec<Rc<Property>>> props;
//         for (auto& shorthand : _value)
//             for (auto& longhand : shorthand->expand(registry, parent, child))
//                 props.lookupOrPutDefault(longhand->registration->name).pushBack(longhand);
//
//         return props.mutIterValue() |
//                Select([](Vec<Rc<Property>>& v) {
//                    return makeRc<ToggleProperty>(v[0]->registration, std::move(v));
//                }) |
//                Collect<Longhands>();
//     }
//
//     void apply(ComputedValues const& parent, ComputedValues& c, ComputationContext const& cx) const override {
//         if (isEmpty(_value))
//             return;
//
//         auto parentProperty = registration->load(parent);
//         usize index = 0;
//         for (auto& p : _value) {
//             index++;
//             if (parentProperty == p->computedValue(parent, c))
//                 break;
//         }
//         _value[index % _value.len()]->apply(parent, c, cx);
//     }
//
//     // https://drafts.csswg.org/cssom/#serialize-a-comma-separated-list
//     Res<> serialize(Io::TextWriter& w) const override {
//         try$(w.writeStr("toggle("s));
//         bool first = true;
//         for (auto const& prop : _value) {
//             if (first) {
//                 first = false;
//             } else {
//                 try$(w.writeStr(", "s));
//             }
//             try$(prop->serialize(w));
//         }
//         return w.writeRune(')');
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("(toggle {})", _value);
//     }
// };

// MARK: Deferred Property -----------------------------------------------------

/// A property that could not be parsed, it's used to store the value
/// as-is and apply it with the cascade and custom properties
struct DeferredProperty : Property {
    Css::Content _value;

    DeferredProperty(LonghandId id, Css::Content value)
        : Property(id), _value(value) {}

    // https://www.w3.org/TR/css-variables-1/#substitute-a-var
    static Res<bool> _expandVariable(Cursor<Css::Sst>& c, Map<Symbol, Css::Content> const& env, Css::Content& out, usize depth) {
        if (depth > 32)
            return Error::invalidData("likely dependency cycle in variables");

        if (not(c->type == Css::Sst::FUNC and
                c->prefix == Css::Token::function("var("))) {
            return Ok(false);
        }

        Cursor<Css::Sst> content = c->content;

        eatWhitespace(content);
        if (content.ended())
            return Ok(true);

        if (content.peek() != Css::Token::IDENT)
            return Ok(true);

        Symbol varName = Symbol::from(content->token.data);

        if (Opt<Css::Content const&> ref = env.lookup(varName)) {
            Cursor<Css::Sst> varContent = *ref;

            if (varContent and varContent.peek() != Css::Token::GUARANTEED_INVALID) {
                try$(_expandContent(varContent, env, out, depth));
                return Ok(true);
            }
        }
        content.next();

        eatWhitespace(content);

        if (not content.skip(Css::Token::COMMA))
            return Ok(true);

        try$(_expandContent(content, env, out, depth));
        return Ok(true);
    }

    static Res<bool> _expandFunction(Cursor<Css::Sst>& c, Map<Symbol, Css::Content> const& env, Css::Content& out, usize depth) {
        if (c->type != Css::Sst::FUNC)
            return Ok(false);

        auto& func = out.emplaceBack(Css::Sst::FUNC);
        func.prefix = c->prefix;
        Cursor<Css::Sst> content = c->content;
        try$(_expandContent(content, env, func.content, depth));

        return Ok(true);
    }

    static Res<> _expandContent(Cursor<Css::Sst>& c, Map<Symbol, Css::Content> const& env, Css::Content& out, usize depth) {
        // NOTE: Hint that we will add all the remaining elements
        out.ensure(out.len() + c.rem());

        while (not c.ended()) {
            if (not try$(_expandVariable(c, env, out, depth + 1)) and
                not try$(_expandFunction(c, env, out, depth))) {
                out.pushBack(*c);
            }

            c.next();
        }

        return Ok();
    }

    Res<Rc<Property>> _expandProperty(ComputedValues& child) const {
        Cursor<Css::Sst> cursor = _value;
        Css::Content out;
        try$(_expandContent(cursor, *child.customProps, out, 0));
        cursor = out;

        // Expanding the variable might have introduced some leading whitespace
        // Accordingly, we need to eat them before parsing
        // See: https://www.w3.org/TR/css-syntax-3/#parse-declaration
        eatWhitespace(cursor);

        auto prop = parseProperty(id, cursor, ParsingMode::STANDARD);
        if (not prop and debugProperties)
            logWarn("failed to parse declaration: {}: {}: {}", propertyName(id), prop, out);

        return prop;
    }

    void apply(ComputedValues const& parent, ComputedValues& c, ComputationContext const& cx) const override {
        auto prop = _expandProperty(c);
        if (not prop)
            return;
        prop.expect()->apply(parent, c, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serialize(w, _value);
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

    DefaultedProperty(LonghandId id, Default value)
        : Property(id), _value(value) {}

    void apply(ComputedValues const& parent, ComputedValues& c, ComputationContext const& cx) const override {
        if (_value == Default::INITIAL) {
            // The initial CSS-wide keyword represents the value
            // defined as the property’s initial value.
            // https://drafts.csswg.org/css-cascade/#initial
            initialProperty(id)->apply(parent, c, cx);
        } else if (_value == Default::INHERIT) {
            // The inherit CSS-wide keyword represents the property’s
            // computed value on the parent element.
            // https://drafts.csswg.org/css-cascade/#inherit
            loadProperty(id, parent)->apply(parent, c, cx);
        } else if (_value == Default::UNSET) {
            // TODO: Re-implement
            // The unset CSS-wide keyword acts as either inherit or initial,
            // depending on whether the property is inherited or not.
            // https://drafts.csswg.org/css-cascade/#inherit-initial
            // if (registration->flags.has(LonghandRegistration::INHERITED))
            //     registration->load(parent)->apply(parent, c, cx);
            // else
            //     registration->initial()->apply(parent, c, cx);

        } else if (_value == Default::REVERT) {
            // TODO: Implement revert()
        } else {
            unreachable();
        }
    }

    bool isDefaulted() const override {
        return true;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("(defaulted {})", _value);
    }
};

// MARK: Bogus Property --------------------------------------------------------

// Represent a property that could not be parsed
// struct BogusProperty : Property {
//     Css::Content _value;
//     Error _error;
//
//     BogusProperty(Properties::, Css::Content value, Error error)
//         : Property(registration),
//           _value(value),
//           _error(error) {}
//
//     bool isBogusProperty() const override {
//         return true;
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{} (error:{})", _value, _error);
//     }
// };

// MARK: Property Registry -----------------------------------------------------

// https://www.w3.org/TR/css-cascade-4/#initial-values
static Opt<Rc<ComputedValues>> _memoInitialComputedValues = NONE;

static Rc<ComputedValues> initialComputedValues() {
    ComputationContext cx{};
    if (not _memoInitialComputedValues) {
        auto initial = makeRc<ComputedValues>();
        for (usize i = 0; i < enumLen<LonghandId>(); i++)
            initialProperty(static_cast<LonghandId>(i))->apply( *initial, *initial, cx);

        _memoInitialComputedValues = Some(initial);
    }
    return makeRc<ComputedValues>(**_memoInitialComputedValues);
}

// https://www.w3.org/TR/css-cascade-4/#inheriting
void inheritsComputedValues(ComputedValues const& parent, ComputedValues& child) {
    // Apply defaulted inheritance fast path for property that supports it.
    child.customProps = parent.customProps;
    child.font = parent.font;
    child.list = parent.list;
    child.text = parent.text;
    child.svgPaint = parent.svgPaint;
    child.table = parent.table;

    // Handle the rest of the properties
    // FIXME
}

Rc<ComputedValues> inheritsComputedValues(ComputedValues const& parent) {
    auto computedValues = initialComputedValues();
    inheritsComputedValues(parent, *computedValues);
    return computedValues;
}

Res<Rc<Property>> _parseDefaulted(LonghandId id, Cursor<Css::Sst>& content) {
        Default value;
        if (content.skip(Css::Token::ident("initial"))) {
            value = Default::INITIAL;
        } else if (content.skip(Css::Token::ident("inherit"))) {
            value = Default::INHERIT;
        } else if (content.skip(Css::Token::ident("unset"))) {
            value = Default::UNSET;
        } else {
            return Error::invalidData("unknown declaration");
        }

        return Ok(makeRc<DefaultedProperty>(id, value));
    }

    Rc<Property> _deferProperty(LonghandId id, Slice<Css::Sst> content) {
        return makeRc<DeferredProperty>(id, content);
    }

    Res<Rc<Property>> parseValue(Symbol const& propertyName, Cursor<Css::Sst> content, Flags<Options> options) {
        Cursor cursor = content;

        auto id = lookupProperty(propertyName).okOr(Error::invalidData("unknown property"));

        id.visit([])

        eatWhitespace(cursor);
        if (auto defaulted = _parseDefaulted(id, cursor))
            return defaulted;

        auto maybeProp = registration->parse(cursor, ParsingMode::STANDARD);
        eatWhitespace(cursor);
        if (not cursor.ended())
            maybeProp = Error::invalidData("unexpected token after property value");

        if (not maybeProp) {
            return Ok(_deferProperty(registration, content));
        }

        return maybeProp;
    }

Res<Rc<Property>> parseDeclaration(Css::Sst const& sst) {
    if (sst != Css::Sst::DECL)
        panic("expected declaration");

    if (sst.token != Css::Token::IDENT)
        panic("expected ident");

    auto propertyName = Symbol::from(sst.token.data);
    Cursor<Css::Sst> content = sst.content;
    auto prop = try$(parseValue(propertyName, content));
    prop->important = sst.flags.has(Css::Sst::IMPORTANT);
    return Ok(prop);
}

Res<Rc<Property>> parseValue(Symbol const& propertyName, Str propertyValue) {
    Css::Lexer lex{propertyValue};
    Diag::Collector diags = Diag::Collector::ignore();
    auto [content, _] = Css::consumeDeclarationValue(lex, diags);
    return parseValue(propertyName, content);
}

Vec<Rc<Property>> parseDeclarations(Css::Content const& sst) {
    Vec<Rc<Property>> res;

    for (auto const& item : sst) {
        if (item != Css::Sst::DECL)
            continue;

        auto prop = parseDeclaration(item);
        if (not prop)
            continue;
        res.pushBack(prop.take());
    }

    return res;
}

Vec<Rc<Property>> parseDeclarations(Css::Sst const& sst) {
    return parseDeclarations(sst.content);
}

Vec<Rc<Property>> parseDeclarations(Str style) {
    Css::Lexer lex{style};
    auto diags = Diag::Collector::ignore();
    auto sst = Css::consumeDeclarationList(lex, diags, true);
    return parseDeclarations(sst);
}

Opt<Rc<Property>> parsePresentationAttribute(Symbol attrName, Str value) {
    // FIXME: Could the caller call toLower ?
    Symbol lower = Symbol::from(toLower(attrName.str()));
    auto id = try$(lookupProperty(lower));

    Css::Lexer lex{value};
    Diag::Collector diags = Diag::Collector::ignore();
    auto [content, _] = Css::consumeDeclarationValue(lex, diags);
    Cursor<Css::Sst> cursor = content;

    return parseProperty(try$(id.is<LonghandId>()), cursor, ParsingMode::PRESENTATION_ATTRIBUTE).ok();
}

export struct _RegisteredPropertySet {
    Map<Symbol, Symbol> _legacyNameAlias;
    Map<Symbol, Rc<LonghandRegistration>> _registrations;
    Map<Symbol, Rc<LonghandRegistration>> _presentationAttributes;

    Vec<Rc<LonghandRegistration>> _individuallyInherited;

    enum struct Options : u8 {
        GENERATE_BOGUS = 1 << 0,
        GENERATE_CUSTOM_PROPERTY = 1 << 1,
        DEFER_UNPARSABLE = 1 << 2,
        ALLOW_DEFAULTING = 1 << 3,
        ALLOW_TOGGLE = 1 << 4,

        TOP_LEVEL = GENERATE_BOGUS | GENERATE_CUSTOM_PROPERTY | DEFER_UNPARSABLE | ALLOW_DEFAULTING | ALLOW_TOGGLE,
    };

    using enum Options;

    auto const& registrations() const {
        return _registrations;
    }

    // MARK: Initial -----------------------------------------------------------

    // MARK: Inherit -----------------------------------------------------------

    // MARK: Value Parsing -----------------------------------------------------

    // MARK: Declaration Parsing -----------------------------------------------

    Res<Rc<Property>> parseDeclaration(Css::Sst const& sst, Flags<Options> options) {
        if (sst != Css::Sst::DECL)
            panic("expected declaration");

        if (sst.token != Css::Token::IDENT)
            panic("expected ident");

        auto propertyName = Symbol::from(sst.token.data);
        Cursor<Css::Sst> content = sst.content;
        auto prop = try$(parseValue(propertyName, content, options));
        prop->important = sst.flags.has(Css::Sst::IMPORTANT);
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
        auto registration = try$(_presentationAttributes.lookup(lower));
        return registration->parsePresentationAttribute(value).ok();
    }
};

} // namespace Vaev::Style
