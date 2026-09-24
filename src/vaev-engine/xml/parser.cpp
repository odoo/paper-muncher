module;

#include <karm/macros>

export module Vaev.Engine:xml.parser;

import Karm.Core;
import Karm.Gc;
import Karm.Logger;

import :dom.document;
import :dom.comment;
import :dom.documentType;

using namespace Karm;

namespace Vaev::Xml {

struct UnresolvedQualifiedName {
    Opt<Symbol> prefix;
    Symbol localName;
};

// Namespace bindings are scoped per element, and the default namespace
// applies to element names but not to unprefixed attributes.
// https://www.w3.org/TR/xml-names/#scoping-defaulting
// https://www.w3.org/TR/xml-names/#ns-qualnames
struct NamespaceContext {
    Opt<Symbol> default_;
    Cow<Map<Symbol, Symbol>> prefixes;

    static NamespaceContext make(Opt<Symbol> default_) {
        NamespaceContext context{};
        context.default_ = default_;
        context.prefixes.cow().put("xml"_sym, Xml::NAMESPACE);
        context.prefixes.cow().put("xmlns"_sym, Xmlns::NAMESPACE);
        return context;
    }

    Res<Dom::QualifiedName> resolveElementName(UnresolvedQualifiedName const& parsedName) const {
        if (not parsedName.prefix)
            return Ok(Dom::QualifiedName{default_, parsedName.localName});

        auto ns = prefixes->lookup(parsedName.prefix.unwrap());
        if (not ns)
            return Error::invalidData("unknown namespace prefix");

        return Ok(Dom::QualifiedName{Some(*ns), parsedName.localName});
    }

    Res<Dom::QualifiedName> resolveAttributeName(UnresolvedQualifiedName const& parsedName) const {
        if (not parsedName.prefix)
            return Ok(Dom::QualifiedName{NONE, parsedName.localName});

        auto ns = prefixes->lookup(parsedName.prefix.unwrap());
        if (not ns)
            return Error::invalidData("unknown namespace prefix");

        return Ok(Dom::QualifiedName{Some(*ns), parsedName.localName});
    }

    void declarePrefix(Symbol prefix, Opt<Symbol> ns) {
        if (prefix == "xml"_sym or prefix == "xmlns"_sym)
            return;

        if (ns)
            prefixes.cow().put(prefix, *ns);
        else
            (void)prefixes.cow().remove(prefix);
    }
};

export template <typename... Ts>
Error _raise(Diag::Collector& diags, Io::LocSpan span, Str format, Ts&&... ts) {
    Io::Args<Ts...> args{std::forward<Ts>(ts)...};
    diags.emit(Diag::Diagnostic::error(Io::format(format, args)).withPrimaryLabel(span));
    return Error::invalidData("invalid xml");
}

export struct XmlParser {
    Gc::Heap& _heap;

    XmlParser(Gc::Heap& heap)
        : _heap(heap) {
    }

    // 2 MARK: Documents
    // https://www.w3.org/TR/xml/#sec-documents
    Res<> parse(Io::SScan& s, Opt<Symbol> const& ns, Dom::Document& doc, Diag::Collector& diags) {
        // document :: = prolog element Misc *

        try$(_parseProlog(s, doc, diags));
        doc.appendChild(try$(_parseElement(s, NamespaceContext::make(ns), diags)));
        while (_parseMisc(s, doc, diags))
            ;

        return Ok();
    }

    // 2.2 MARK: Characters
    // https://www.w3.org/TR/xml/#charsets

    static constexpr auto RE_CHAR =
        '\x09'_re | '\x0A'_re | '\x0D'_re | Re::range(0x20, 0xD7FF) |
        Re::range(0xE000, 0xFFFD) | Re::range(0x10000, 0x10FFFF);

    // 2.3 MARK: Common Syntactic Constructs
    // https://www.w3.org/TR/xml/#sec-common-syn

    static constexpr auto RE_S =
        Re::single(' ', '\t', '\r', '\n');

    static bool _isNameStartChar(Rune r) {
        Array RANGES = {
            Pair<Rune, Rune>{':', ':'},
            Pair<Rune, Rune>{'A', 'Z'},
            Pair<Rune, Rune>{'_', '_'},
            Pair<Rune, Rune>{'a', 'z'},
            Pair<Rune, Rune>{0xC0, 0xD6},
            Pair<Rune, Rune>{0xD8, 0xF6},
            Pair<Rune, Rune>{0xF8, 0x2FF},
            Pair<Rune, Rune>{0x370, 0x37D},
            Pair<Rune, Rune>{0x37F, 0x1FFF},
            Pair<Rune, Rune>{0x200C, 0x200D},
            Pair<Rune, Rune>{0x2070, 0x218F},
            Pair<Rune, Rune>{0x2C00, 0x2FEF},
            Pair<Rune, Rune>{0x3001, 0xD7FF},
            Pair<Rune, Rune>{0xF900, 0xFDCF},
            Pair<Rune, Rune>{0xFDF0, 0xFFFD},
            Pair<Rune, Rune>{0x10000, 0xEFFFF},
        };

        for (auto range : RANGES)
            if (r >= range.v0 and r <= range.v1)
                return true;

        return false;
    }

    static constexpr auto RE_NAME_START_CHAR = Re::ctype(_isNameStartChar);

    static bool _isNameChar(Rune r) {
        Array RANGES = {
            Pair<Rune, Rune>{'-', '-'},
            Pair<Rune, Rune>{'.', '.'},
            Pair<Rune, Rune>{'0', '9'},
            Pair<Rune, Rune>{0xB7, 0xB7},
            Pair<Rune, Rune>{0x0300, 0x036F},
            Pair<Rune, Rune>{0x203F, 0x2040},
        };

        for (auto range : RANGES)
            if (r >= range.v0 and r <= range.v1)
                return true;

        return false;
    }

    static constexpr auto RE_NAME_CHAR = RE_NAME_START_CHAR | Re::ctype(_isNameChar);

    static constexpr auto RE_NAME = RE_NAME_START_CHAR & Re::zeroOrMore(RE_NAME_CHAR);

    static Res<UnresolvedQualifiedName> _parseQualifiedName(Str name, Diag::Collector& diags) {
        Opt<usize> separator = NONE;

        for (usize i = 0; i < name.len(); i++) {
            if (name[i] != ':')
                continue;

            if (separator)
                return Error::invalidData("expected a single namespace separator");

            separator = Some(i);
        }

        if (not separator)
            return Ok(UnresolvedQualifiedName{NONE, Symbol::from(name)});

        auto separatorIndex = separator.unwrap();
        if (separatorIndex == 0 or separatorIndex + 1 == name.len())
            return Error::invalidData("expected namespace prefix and local name");

        return Ok(UnresolvedQualifiedName{
            Some(Symbol::from(sub(name, 0, separatorIndex))),
            Symbol::from(sub(name, separatorIndex + 1, name.len())),
        });
    }

    Res<UnresolvedQualifiedName> _parseQualifiedName(Io::SScan& s, Diag::Collector& diags) {
        return _parseQualifiedName(try$(_parseName(s, diags)), diags);
    }

    void _eatS(Io::SScan& s) {
        // S ::= (#x20 | #x9 | #xD | #xA)+
        s.eat(Re::oneOrMore(RE_S));
    }

    Res<Str> _parseName(Io::SScan& s, Diag::Collector& diags) {
        // Name ::= NameStartChar (NameChar)*

        auto name = s.token(RE_NAME);
        if (isEmpty(name))
            return _raise(diags, Io::LocSpan::single(s.loc()), "expected name");
        return Ok(name);
    }

    // 2.4 MARK: Character Data and Markup
    // https://www.w3.org/TR/xml/#syntax

    static constexpr auto RE_CHARDATA = Re::negate(Re::single('<', '&'));

    Res<> _parseCharData(Io::SScan& s, StringBuilder& sb, Diag::Collector& diags) {
        // CharData ::= [^<&]* - ([^<&]* ']]>' [^<&]*)

        bool any = false;

        while (
            s.ahead(RE_CHARDATA) and
            not s.ahead("]]>"_re) and
            not s.ended()
        ) {
            sb.append(s.next());
            any = true;
        }

        if (not any)
            return _raise(diags, Io::LocSpan::single(s.loc()), "expected character data");

        return Ok();
    }

    // 2.5 MARK: Comments
    // https://www.w3.org/TR/xml/#sec-comments
    static constexpr auto RE_COMMENT_START = "<!--"_re;
    static constexpr auto RE_COMMENT_END = "-->"_re;

    Res<Gc::Ref<Dom::Comment>> _parseComment(Io::SScan& s, Diag::Collector& diags) {
        // 	Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'

        if (not s.skip(RE_COMMENT_START))
            unreachable();

        StringBuilder sb;
        while (not s.ahead(RE_COMMENT_END) and not s.ended()) {
            auto chrs = s.token(RE_CHAR);
            if (isEmpty(chrs))
                return _raise(diags, Io::LocSpan::single(s.loc()), "expected character data");
            sb.append(chrs);
        }

        if (not s.skip(RE_COMMENT_END))
            return _raise(diags, Io::LocSpan::single(s.loc()), "expected '-->'");

        return Ok(_heap.alloc<Dom::Comment>(sb.take()));
    }

    // 2.6 MARK: Processing Instructions
    // https://www.w3.org/TR/xml/#sec-pi

    static constexpr auto RE_PI_START = "<?"_re;
    static constexpr auto RE_PI_END = "?>"_re;

    Res<> _parsePi(Io::SScan& s, Diag::Collector& diags) {
        // PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>

        if (not s.skip(RE_PI_START))
            unreachable();

        try$(_parsePiTarget(s, diags));

        while (not s.ahead(RE_PI_END) and not s.ended()) {
            auto chrs = s.token(RE_CHAR);
            if (isEmpty(chrs))
                return _raise(diags, Io::LocSpan::single(s.loc()), "expected character data");
        }

        if (not s.skip(RE_PI_END))
            return _raise(diags, Io::LocSpan::single(s.loc()), "expected '?>'");

        return Ok();
    }

    Res<> _parsePiTarget(Io::SScan& s, Diag::Collector& diags) {
        // PITarget ::= Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))

        auto startLoc = s.loc();
        auto name = try$(_parseName(s, diags));
        if (eqCi(name, "xml"s))
            return _raise(diags, {.start = startLoc, .end = s.loc()}, "expected name to not be 'xml'") ;
        return Ok();
    }

    // 2.7 MARK: CDATA Sections
    // https://www.w3.org/TR/xml/#sec-cdata-sect

    static constexpr auto RE_CDATA_START = "<![CDATA["_re;
    static constexpr auto RE_CDATA_END = "]]>"_re;

    Res<> _parseCDSect(Io::SScan& s, StringBuilder& sb, Diag::Collector& diags) {
        // CDStart ::= '<![CDATA['
        // CData ::= (Char* - (Char* ']]>' Char*))
        // CDEnd ::= ']]>'

        if (not s.skip(RE_CDATA_START))
            unreachable();

        while (s.match(RE_CDATA_END) == Match::NO and not s.ended())
            sb.append(s.next());

        if (not s.skip(RE_CDATA_END))
            return _raise(diags, Io::LocSpan::single(s.loc()), "expected ']]>") ;

        return Ok();
    }

    // 2.8 MARK: Prolog and Document Type Declaration
    // https://www.w3.org/TR/xml/#sec-prolog-dtd

    static constexpr auto RE_XML_DECL_START = "<?xml"_re;
    static constexpr auto RE_XML_DECL_VERSION = Re::chain("1."_re, Re::oneOrMore(Re::digit()));
    static constexpr auto RE_XML_DECL_ENCODING = Re::chain(Re::alpha(), Re::zeroOrMore(Re::word() | "."_re | "-"_re));
    static constexpr auto RE_XML_DECL_STANDALONE = "yes"_re | "no"_re;

    Res<Str> _parseXmlDeclAttr(Io::SScan& s, Re::Expr auto name, Re::Expr auto value) {
        auto rollback = s.rollbackPoint();

        _eatS(s);

        if (not s.skip(name))
            return Error::invalidData("expected attribute");

        _eatS(s);

        if (not s.skip('='))
            return Error::invalidData("expected '='");

        _eatS(s);

        auto quote = s.next();
        if (quote != '"' and quote != '\'')
            return Error::invalidData("expected '\"' or '''");

        auto result = s.token(value);
        if (not result)
            return Error::invalidData("expected attribute value");

        if (s.peek() != quote)
            return Error::invalidData("expected closing quote");
        s.next();

        rollback.disarm();
        return Ok(result);
    }

    Res<> _parseXmlDecl(Io::SScan& s, Dom::Document& doc) {
        // XMLDecl ::= '<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'

        if (not s.skip(RE_XML_DECL_START))
            return Error::invalidData("expected '<?xml'");

        // versionInfo ::= S 'version' Eq ("'" VersionNum "'" | '"' VersionNum '"')
        auto version = try$(_parseXmlDeclAttr(s, "version"_re, RE_XML_DECL_VERSION));
        logWarnIf(version != "1.0", "Version {} not supported, treating this document as a 1.0 document", version);
        doc.xmlVersion = version;

        // EncodingDecl ::= S 'encoding' Eq ('"' EncName '"' | "'" EncName "'" )
        auto encoding = _parseXmlDeclAttr(s, "encoding"_re, RE_XML_DECL_ENCODING);
        if (encoding.has())
            doc.xmlEncoding = encoding.unwrap();

        // SDDecl ::= S 'standalone' Eq (("'" ('yes' | 'no') "'") | ('"' ('yes' | 'no') '"'))
        auto standalone = _parseXmlDeclAttr(s, "standalone"_re, RE_XML_DECL_STANDALONE);
        if (standalone.has())
            doc.xmlStandalone = standalone.unwrap();

        _eatS(s);

        if (not s.skip(RE_PI_END))
            return Error::invalidData("expected '?>'");

        return Ok();
    }

    bool _miscAhead(Io::SScan& s) {
        return s.ahead(RE_COMMENT_START) or s.ahead(RE_PI_START) or s.ahead(RE_S);
    }

    Res<> _parseMisc(Io::SScan& s, Dom::Node& parent, Diag::Collector& diags) {
        // Misc ::= Comment | PI | S

        if (s.ahead(RE_COMMENT_START)) {
            auto c = try$(_parseComment(s, diags));
            parent.appendChild(c);
        } else if (s.ahead(RE_PI_START))
            try$(_parsePi(s, diags));
        else if (s.ahead(RE_S))
            _eatS(s);
        else
            unreachable();

        return Ok();
    }

    Res<> _parseProlog(Io::SScan& s, Dom::Document& doc, Diag::Collector& diags) {
        // prolog ::= XMLDecl? Misc* (doctypedecl Misc*)?
        if (s.ahead(RE_XML_DECL_START))
            try$(_parseXmlDecl(s, doc));

        while (_miscAhead(s))
            try$(_parseMisc(s, doc, diags));

        if (s.ahead(RE_DOCTYPE_START)) {
            auto doctype = try$(_parseDoctype(s, diags));
            doc.appendChild(doctype);

            while (_miscAhead(s))
                try$(_parseMisc(s, doc, diags));
        }

        return Ok();
    }

    static constexpr auto RE_DOCTYPE_START = "<!DOCTYPE"_re;

    Res<Gc::Ref<Dom::DocumentType>> _parseDoctype(Io::SScan& s, Diag::Collector& diags) {
        // doctypedecl ::= '<!DOCTYPE' S Name (S ExternalID)? S? ('[' intSubset ']' S?)? '>'
        auto rollback = s.rollbackPoint();

        if (not s.skip(RE_DOCTYPE_START))
            unreachable();

        auto docType = _heap.alloc<Dom::DocumentType>();

        _eatS(s);

        docType->name = Symbol::from(try$(_parseName(s, diags)));

        _eatS(s);

        // FIXME: Investigate
        (void)_parseExternalId(s, *docType, diags);

        _eatS(s);
        if (not s.skip('>'))
            return Error::invalidData("expected '>'");

        rollback.disarm();
        return Ok(docType);
    }

    // 2.9 MARK: Standalone Document Declaration
    // https://www.w3.org/TR/xml/#sec-rmd

    // 3 MARK: Logical Structures
    // https://www.w3.org/TR/xml/#sec-logical-struct

    Res<Gc::Ref<Dom::Element>> _parseElement(Io::SScan& s, Opt<Symbol> const& ns, Diag::Collector& diags) {
        return _parseElement(s, NamespaceContext::make(ns), diags);
    }

    Res<Gc::Ref<Dom::Element>> _parseElement(Io::SScan& s, NamespaceContext const& context, Diag::Collector& diags) {
        // element ::= EmptyElemTag | STag content ETag
        // EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'
        // STag ::= '<' Name (S Attribute)* S? '>'

        if (not s.skip('<'))
            unreachable();

        auto parsedName = try$(_parseQualifiedName(s, diags));
        _eatS(s);

        auto childContext = try$(_parseNamespaceContext(s, context, diags));
        auto el = _heap.alloc<Dom::Element>(try$(childContext.resolveElementName(parsedName)));

        while (not (s.ahead('>'_re | "/>"_re) and not s.ended())) {
            try$(_parseAttribute(s, *el, childContext, diags));
            _eatS(s);
        }

        if (s.ahead('>'_re)) {
            try$(_parseContent(s, childContext, *el, diags));
            try$(_parseEndTag(s, childContext, *el, diags));
            return Ok(el);
        }

        if (s.ahead("/>"_re))
            return Ok(el);

        return _raise(diags, Io::LocSpan::single(s.loc()), "unexpected EOF");
    }

    // 3.1 MARK: Start-Tags, End-Tags, and Empty-Element Tags
    // https://www.w3.org/TR/xml/#sec-starttags

    Res<> _parseAttribute(Io::SScan& s, Dom::Element& el, NamespaceContext const& context, Diag::Collector& diags) {
        // Attribute ::= Name Eq AttValue

        auto parsedName = try$(_parseQualifiedName(s, diags));

        if (not s.skip('='))
            return Error::invalidData("expected '='");

        auto value = try$(_parseAttValue(s, diags));

        // FIXME: The parsing allows rollback so it can be that a warning is emitted when setting an attribute of an element
        // that won't compose the final dom (due to a rollback after a failed parsing)
        el.setAttribute(try$(context.resolveAttributeName(parsedName)), value);

        return Ok();
    }

    Res<String> _parseAttValue(Io::SScan& s, Diag::Collector& diags) {
        // AttValue ::= '"' ([^<&"] | Reference)* '"'
        //              |  "'" ([^<&'] | Reference)* "'"

        StringBuilder sb;

        auto quote = s.next();
        if (quote != '"' and quote != '\'')
            return _raise(diags, Io::LocSpan::single(s.loc()), "expected '\"' or '''");

        while (s.peek() != quote and not s.ended()) {
            if (auto r = _parseReference(s, diags))
                sb.append(r.unwrap());
            else
                sb.append(s.next());
        }

        if (s.peek() != quote)
            return _raise(diags, Io::LocSpan::single(s.loc()), "expected closing quote");

        s.next();

        return Ok(sb.take());
    }

    Res<> _parseEndTag(Io::SScan& s, NamespaceContext const& context, Dom::Element& el, Diag::Collector& diags) {
        // '</' Name S? '>'

        auto rollback = s.rollbackPoint();

        if (not s.skip("</"_re))
            return Error::invalidData("expected '</'");

        if (try$(context.resolveElementName(try$(_parseQualifiedName(s, diags)))) != el.qualifiedName)
            return Error::invalidData("expected end tag name to match start tag name");

        _eatS(s);

        if (not s.skip('>'))
            return Error::invalidData("expected '>'");

        rollback.disarm();
        return Ok();
    }

    Res<> _parseContentItem(Io::SScan& s, NamespaceContext const& context, Dom::Element& el, Diag::Collector& diags) {
        // (element | Reference | CDSect | PI | Comment)

        if (s.ahead(RE_PI_START)) {
            (void)try$(_parsePi(s, diags));
            // FIXME: Use diags
            logWarn("ignoring processing instruction");
            return Ok();
        }

        if (s.ahead(RE_COMMENT_START)) {
            auto r = try$(_parseComment(s, diags));
            el.appendChild(r);
            return Ok();
        }

        if (s.ahead('<')) {
            auto r = try$(_parseElement(s, context, diags));
            el.appendChild(r);
            return Ok();
        }

        // FIXME: Diags
        return Error::invalidData("expected content item");
    }

    Res<> _parseContent(Io::SScan& s, NamespaceContext const& context, Dom::Element& el, Diag::Collector& diags) {
        // content ::= CharData? ((element | Reference | CDSect | PI | Comment) CharData?)*

        try$(_parseText(s, el, diags));
        while (_parseContentItem(s, context, el, diags))
            try$(_parseText(s, el, diags));

        return Ok();
    }

    Res<> _parseTextItem(Io::SScan& s, StringBuilder& sb, Diag::Collector& diags) {
        if (s.ahead('&')) {
            auto r = try$(_parseReference(s, diags));
            sb.append(r);
            return Ok();
        }

        if (s.ahead(RE_CDATA_START)) {
            return _parseCDSect(s, sb, diags);
        }

        return _parseCharData(s, sb, diags);
    }

    [[gnu::flatten]] Res<> _parseText(Io::SScan& s, Dom::Element& el, Diag::Collector& diags) {
        StringBuilder sb;

        // Sprinkles of speculative parsing
        while (_parseTextItem(s, sb, diags))
            ;

        auto te = sb.take();
        if (te)
            el.appendChild(_heap.alloc<Dom::Text>(te));

        return Ok();
    }

    // 4.1 MARK: Character and Entity References
    // https://www.w3.org/TR/xml/#NT-CharRef

    Res<Rune> _parseCharRef(Io::SScan& s, Diag::Collector& diags) {
        // CharRef ::= '&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';'

        if (not s.skip("&#"_re))
            unreachable();

        Rune r = REPLACEMENT;

        if (s.skip('x')) {
            auto startLoc = s.loc();
            auto val = Io::atoi(s, {.base = 16});
            if (not val)
                _raise(diags, Io::LocSpan::single(s.loc()), "expected hexadecima number");
            r = val.unwrap();
        } else {
            auto startLoc = s.loc();
            auto val = Io::atoi(s, {.base = 10});
            if (not val)
                return Error::invalidData("expected decimal number");
            r = val.unwrap();
        }

        if (not s.skip(';'))
            return Error::invalidData("expected ';'");

        return Ok(r);
    }

    Res<Rune> _parseEntityRef(Io::SScan& s, Diag::Collector& diags) {
        // EntityRef ::= '&' Name ';'

        auto rollback = s.rollbackPoint();
        auto startLoc = s.loc();

        if (not s.skip('&'))
            return _raise(diags, Io::LocSpan::single(s.loc()), "expected '&'");

        auto name = try$(_parseName(s, diags));

        if (not s.skip(';'))
            return _raise(diags, Io::LocSpan::single(s.loc()), "expected ';'");

        rollback.disarm();
        if (name == "lt")
            return Ok('<');
        else if (name == "gt")
            return Ok('>');
        else if (name == "amp")
            return Ok('&');
        else if (name == "apos")
            return Ok('\'');
        else if (name == "quot")
            return Ok('"');

        // NOTE: (workaround) we don't support XML fully so nbsp can't be loaded from a DOCTYPE as its supposed to
        else if (name == "nbsp")
            return Ok(160);

        rollback.arm();
        return _raise(diags, {.start = startLoc, .end = s.loc()}, "unknown entity reference");
    }

    Res<Rune> _parseReference(Io::SScan& s, Diag::Collector& diags) {
        // Reference ::= EntityRef | CharRef

        if (not s.ahead('&'))
            unreachable();

        if (s.ahead("&#"_re)) {
            return _parseCharRef(s, diags);
        } else {
            return _parseEntityRef(s, diags);
        }
    }

    // 4.2 MARK: Entity Declarations
    // https://www.w3.org/TR/xml/#sec-entity-decl

    Res<> _parseExternalId(Io::SScan& s, Dom::DocumentType& docType, Diag::Collector& diags) {
        // ExternalID ::= 'SYSTEM' S SystemLiteral | 'PUBLIC' S PubidLiteral S SystemLiteral

        if (s.skip("SYSTEM"_re)) {
            _eatS(s);
            // NOSPEC: We are parsing the system literal as att value
            docType.systemId = try$(_parseAttValue(s, diags));
            return Ok();
        } else if (s.skip("PUBLIC"_re)) {
            // NOSPEC: We are parsing the public and system literals as att values
            _eatS(s);
            docType.publicId = try$(_parseAttValue(s, diags));
            _eatS(s);
            docType.systemId = try$(_parseAttValue(s, diags));
            return Ok();
        } else {
            return _raise(diags, Io::LocSpan::single(s.loc()), "expected 'SYSTEM' or 'PUBLIC'");
        }
    }

    // MARK: 6.2 Namespace Defaulting
    // https://www.w3.org/TR/xml-names/#dt-defaultNS
    // https://www.w3.org/TR/xml-names/#scoping-defaulting
    // NOTE: Basically same code as attribute parsing, but we need to check for the namespace before parsing the attributes

    Res<NamespaceContext> _parseNamespaceContext(Io::SScan& s, NamespaceContext const& originalContext, Diag::Collector& diags) {
        auto context = originalContext;

        while (not s.ahead(">"_re) and not s.ahead("/>"_re) and not s.ended()) {
            auto parsedName = try$(_parseQualifiedName(s, diags));

            if (not s.skip('=')) {
                return _raise(diags, Io::LocSpan::single(s.loc()), "expected '='");
            }

            auto value = try$(_parseAttValue(s, diags));

            if (not parsedName.prefix and parsedName.localName == "xmlns"_sym)
                context.default_ = Some(Symbol::from(value));
            else if (parsedName.prefix == "xmlns"_sym)
                context.declarePrefix(parsedName.localName, Some(Symbol::from(value)));

            _eatS(s);
        }

        return Ok(context);
    }
};

} // namespace Vaev::Xml
