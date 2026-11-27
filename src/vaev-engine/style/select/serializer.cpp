module;

#include <karm/macros>

export module Vaev.Engine:style.select.serializer;

import Karm.Core;

import :css;
import :css.values;
import :dom.element;
import :style.select.pseudoClass;
import :style.select.qualifiedName;
import :style.select.selectors;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// https://drafts.csswg.org/cssom/#serializing-selectors

export Res<> serialize(Io::TextWriter& w, SelectorList const& selector);

static Res<> _serialize(Io::TextWriter& w, String const& value) {
    return Css::serializeValue(w, value);
}

static Res<> _serializeList(Io::TextWriter& w, auto const& items) {
    for (auto const& [item, i] : iter(items) | Index()) {
        if (i > 0)
            try$(w.writeStr(", "s));
        try$(_serialize(w, item));
    }
    return Ok();
}

static Res<> _serialize(Io::TextWriter& w, QualifiedNameSelector const& selector, bool isAttribute) {
    try$(selector.ns.visit(
        [&](NullNamespace const&) -> Res<> {
            if (not isAttribute)
                try$(w.writeStr("|"s));
            return Ok();
        },
        [&](Universal const&) -> Res<> {
            if (isAttribute)
                try$(w.writeStr("*|"s));
            return Ok();
        },
        [&](Symbol const& ns) -> Res<> {
            return Io::format(w, "{}|", ns);
        }
    ));

    return selector.name.visit(
        [&](Universal const&) {
            return w.writeStr("*"s);
        },
        [&](Symbol const& name) {
            return w.writeStr(name.str());
        }
    );
}

static Res<> _serialize(Io::TextWriter& w, TypeSelector const& selector) {
    return _serialize(w, selector.qualifiedName, false);
}

static Str _attributeOperator(AttributeSelector::Match match) {
    switch (match) {
    case AttributeSelector::EXACT:
        return "=";
    case AttributeSelector::CONTAINS:
        return "~=";
    case AttributeSelector::HYPHENATED:
        return "|=";
    case AttributeSelector::STR_START_WITH:
        return "^=";
    case AttributeSelector::STR_END_WITH:
        return "$=";
    case AttributeSelector::STR_CONTAIN:
        return "*=";
    default:
        unreachable();
    }
}

static Res<> _serialize(Io::TextWriter& w, AttributeSelector const& selector) {
    try$(w.writeStr("["s));
    try$(_serialize(w, selector.qualifiedName, true));
    if (selector.match != AttributeSelector::PRESENT) {
        try$(w.writeStr(_attributeOperator(selector.match)));
        try$(Css::serializeValue(w, selector.value));
        if (selector.case_ == AttributeSelector::INSENSITIVE)
            try$(w.writeStr(" i"s));
    }
    return w.writeStr("]"s);
}

static Res<> _serialize(Io::TextWriter& w, ClassSelector const& selector) {
    return Io::format(w, ".{}", selector.class_);
}

static Res<> _serialize(Io::TextWriter& w, IdSelector const& selector) {
    return Io::format(w, "#{}", selector.id);
}

static Res<> _serialize(Io::TextWriter& w, PseudoElementSelector const& selector) {
    return w.writeStr(selector.type.str());
}

static Res<> _serialize(Io::TextWriter& w, PseudoClassSelector const& selector) {
    return Io::format(w, ":{}", serializePseudoClass(selector.pseudoClass));
}

static Res<> _serializeFunction(Io::TextWriter& w, Str name, auto const& args) {
    try$(Io::format(w, ":{}(", name));
    try$(_serializeList(w, args));
    return w.writeStr(")"s);
}

static Res<> _serialize(Io::TextWriter& w, MatchAnyPseudoClass const& selector) {
    return _serializeFunction(w, "is", selector.selectors);
}

static Res<> _serialize(Io::TextWriter& w, NegationPseudoClass const& selector) {
    return _serializeFunction(w, "not", selector.selectors);
}

static Res<> _serialize(Io::TextWriter& w, SpecificityAdjustmentPseudoClass const& selector) {
    return _serializeFunction(w, "where", selector.selectors);
}

static Res<> _serialize(Io::TextWriter& w, RelationalPseudoClass const& selector) {
    return _serializeFunction(w, "has", selector.selectors);
}

static Res<> _serialize(Io::TextWriter& w, DirPseudoClass const& selector) {
    return Io::format(w, ":dir({})", selector.dir == Dir::LTR ? "ltr"s : "rtl"s);
}

static Res<> _serialize(Io::TextWriter& w, LangPseudoClass const& selector) {
    return _serializeFunction(w, "lang", selector.langs);
}

static Res<> _serialize(Io::TextWriter& w, ChildIndexPseudoClass const& selector) {
    switch (selector.type) {
    case ChildIndexPseudoClass::NTH_CHILD:
        try$(w.writeStr(":nth-child"s));
        break;
    case ChildIndexPseudoClass::NTH_LAST_CHILD:
        try$(w.writeStr(":nth-last-child"s));
        break;
    case ChildIndexPseudoClass::FIRST_CHILD:
        return w.writeStr(":first-child"s);
    case ChildIndexPseudoClass::LAST_CHILD:
        return w.writeStr(":last-child"s);
    case ChildIndexPseudoClass::ONLY_CHILD:
        return w.writeStr(":only-child"s);
    default:
        unreachable();
    }

    try$(Io::format(w, "({}", selector.anb));
    if (selector.s) {
        try$(w.writeStr(" of "s));
        try$(serialize(w, selector.s));
    }
    return w.writeStr(")"s);
}

static Res<> _serialize(Io::TextWriter& w, TypedChildIndexPseudoClass const& selector) {
    switch (selector.type) {
    case TypedChildIndexPseudoClass::NTH_OF_TYPE:
        return Io::format(w, ":nth-of-type({})", selector.anb);
    case TypedChildIndexPseudoClass::NTH_LAST_OF_TYPE:
        return Io::format(w, ":nth-last-of-type({})", selector.anb);
    case TypedChildIndexPseudoClass::FIRST_OF_TYPE:
        return w.writeStr(":first-of-type"s);
    case TypedChildIndexPseudoClass::LAST_OF_TYPE:
        return w.writeStr(":last-of-type"s);
    case TypedChildIndexPseudoClass::ONLY_OF_TYPE:
        return w.writeStr(":only-of-type"s);
    default:
        unreachable();
    }
}

static Res<> _serialize(Io::TextWriter& w, NestingSelector const&) {
    return w.writeStr("&"s);
}

static Res<> _serialize(Io::TextWriter& w, SubclassSelector const& selector) {
    return selector.visit([&](auto const& s) {
        return _serialize(w, s);
    });
}

static Res<> _serialize(Io::TextWriter& w, CompoundSelector const& selector) {
    if (selector.type)
        try$(_serialize(w, *selector.type));
    for (auto const& s : selector.subclasses)
        try$(_serialize(w, s));
    return Ok();
}

static Res<> _serialize(Io::TextWriter& w, PseudoCompoundSelector const& selector) {
    try$(_serialize(w, selector.pseudoElement));
    for (auto const& s : selector.pseudoClasses)
        try$(_serialize(w, s));
    return Ok();
}

static Res<> _serialize(Io::TextWriter& w, ComplexSelectorUnit const& selector) {
    if (selector.selector)
        try$(_serialize(w, *selector.selector));
    for (auto const& s : selector.pseudo)
        try$(_serialize(w, s));
    return Ok();
}

static Res<> _serialize(Io::TextWriter& w, ComplexSelector const& selector) {
    for (auto const& unit : selector.lhs) {
        try$(_serialize(w, static_cast<ComplexSelectorUnit const&>(unit)));
        try$(w.writeStr(serializeCombinator(unit.combinator)));
    }
    return _serialize(w, selector.rhs);
}

static Res<> _serialize(Io::TextWriter& w, RelativeComplexSelector const& selector) {
    for (auto const& [unit, i] : iter(selector.selectors) | Index()) {
        auto combinator = serializeCombinator(unit.combinator);
        if (i > 0)
            try$(w.writeStr(combinator));
        else if (unit.combinator != SelectorCombinator::DESCENDANT)
            try$(w.writeStr(Str{next(combinator, 1)}));
        try$(_serialize(w, static_cast<ComplexSelectorUnit const&>(unit)));
    }
    return Ok();
}

export Res<> serialize(Io::TextWriter& w, SelectorList const& selector) {
    return _serializeList(w, selector);
}

export String serialize(SelectorList const& selector) {
    Io::StringWriter sw;
    serialize(sw, selector).expect("serializing to a string");
    return sw.take();
}

} // namespace Vaev::Style
