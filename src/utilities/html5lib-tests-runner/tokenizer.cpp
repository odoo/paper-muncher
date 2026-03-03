module;

#include <karm/macros>

export module Html5LibTest:tokenizer;

import Karm.Core;
import Karm.Diag;
import Karm.Gc;
import Karm.Ref;

import Vaev.Engine;

import :testResult;

using namespace Karm;
using namespace Vaev;
using namespace Vaev::Html;

namespace Html5LibTest::Tokenizer {

struct TokenCollectorSink : HtmlSink {
    HtmlSink* sink;
    Vec<HtmlToken> collected{};

    TokenCollectorSink(HtmlSink* sink)
        : sink(sink) {}

    void accept(HtmlToken& token, Diag::Collector& diags) override {
        collected.pushBack(token);
        sink->accept(token, diags);
    }
};

static String escape(Str s) {
    Io::StringWriter sw;
    Io::Emit emit = sw;
    Json::escape(emit, s);
    return sw.take();
}

// NOTE: This works a bit differently from normal JSON unescaping since we must output invalid
//       surrogate pairs as two runes instead of using a replacement character.
String unescape(Str str) {
    Io::SScan s = {str};
    Io::StringWriter sw{str.len()};
    while (not s.ended()) {
        if (s.skip('\\') and s.skip('u')) {
            auto parseCodeUnit = [&]() -> Res<Utf16::Unit> {
                auto hex = s.token(Re::exactly(4, Re::xdigit()));
                if (not hex)
                    return Error::invalidData();

                auto unit = Io::atou(hex, {.base = 16});
                if (not unit)
                    return Error::invalidData("expected 4 hex digits after \\u");

                return Ok(*unit);
            };

            auto high = parseCodeUnit().unwrap();

            Utf16::One units;
            units.put(high);

            if (Utf16::unitLen(high) == 2) {
                if (s.skip('\\')) {
                    if (s.skip('u')) {
                        auto low = parseCodeUnit().unwrap();
                        units.put(low);
                    } else {
                        panic("malformed unicode escape");
                    }
                }
            }

            Rune rune;
            Cursor cursor{units.buf(), units.len()};

            if (Utf16::decodeUnit(rune, cursor)) {
                sw.append(rune);
            } else {
                sw.append(high);
            }

            continue;

            if (auto u = Io::atou(s, {.base = 16})) {
                sw.append(*u);
                continue;
            }
        }
        sw.append(s.next());
    }
    return sw.take();
}

HtmlLexer::State stringToState(Str s) {
    static constexpr Array states = {
        Tuple{"Data state"s, HtmlLexer::DATA},
        Tuple{"PLAINTEXT state"s, HtmlLexer::PLAINTEXT},
        Tuple{"RCDATA state"s, HtmlLexer::RCDATA},
        Tuple{"RAWTEXT state"s, HtmlLexer::RAWTEXT},
        Tuple{"Script data state"s, HtmlLexer::SCRIPT_DATA},
        Tuple{"CDATA section state"s, HtmlLexer::CDATA_SECTION},
    };

    for (auto const& [name, state] : states) {
        if (s == name)
            return state;
    }

    notImplemented();
}

Serde::Array serializeTokens(Slice<HtmlToken> tokens, bool doubleEscaped) {
    Serde::Array serializedTokens;
    StringBuilder builder;

    auto formatStr = [&](Str const& val) -> String {
        return doubleEscaped ? escape(val) : String{val};
    };

    auto formatSym = [&](Symbol const& val) -> String {
        return formatStr(val.str());
    };

    auto flushCharacterBuffer = [&]() {
        if (builder.len() > 0) {
            Serde::Array charToken;
            charToken.pushBack("Character"s);
            charToken.pushBack(formatStr(builder.take()));
            serializedTokens.pushBack(std::move(charToken));
        }
    };

    for (auto const& t : tokens) {
        if (t.type == HtmlToken::CHARACTER) {
            builder.append(t.rune);
            continue;
        }

        flushCharacterBuffer();

        Serde::Array serializedToken;

        switch (t.type) {
        case HtmlToken::DOCTYPE:
            serializedToken.pushBack("DOCTYPE"s);
            serializedToken.pushBack(t.name.map(formatSym));
            serializedToken.pushBack(t.publicIdent ? formatStr(*t.publicIdent) : Serde::Value{NONE});
            serializedToken.pushBack(t.systemIdent ? formatStr(*t.systemIdent) : Serde::Value{NONE});
            serializedToken.pushBack(not t.forceQuirks);
            break;

        case HtmlToken::START_TAG: {
            serializedToken.pushBack("StartTag"s);
            serializedToken.pushBack(t.name.map(formatSym));

            Serde::Object attrs;
            for (auto const& attr : t.attrs) {
                attrs.put(formatStr(attr.name.str()), formatStr(attr.value.str()));
            }
            serializedToken.pushBack(std::move(attrs));

            if (t.selfClosing) {
                serializedToken.pushBack(true);
            }
            break;
        }

        case HtmlToken::END_TAG:
            serializedToken.pushBack("EndTag"s);
            serializedToken.pushBack(t.name.map(formatSym));

            break;

        case HtmlToken::COMMENT:
            serializedToken.pushBack("Comment"s);
            serializedToken.pushBack(formatStr(t.data.str()));
            break;

        default:
            continue; // Ignore other token types silently as per original logic
        }

        serializedTokens.pushBack(std::move(serializedToken));
    }

    flushCharacterBuffer();

    return serializedTokens;
}

// https://github.com/html5lib/html5lib-tests/blob/master/tokenizer/README.md
export Res<Result> run(Str inputStr) {
    auto testObject = try$(Json::parse(inputStr)).asObject();

    auto input = try$(testObject.lookup("input"s).okOr(Error::invalidData("missing input"))).asStr();
    auto output = try$(testObject.lookup("output"s).okOr(Error::invalidData("missing output"))).asArray();
    auto doubleEscaped = testObject.lookup("doubleEscaped"s).unwrapOr(false);
    if (doubleEscaped) {
        input = unescape(input);
    }
    auto initialStates = testObject.lookup("initialStates"s).unwrapOr(Serde::Array{"Data state"s}).asArray();
    Opt<HtmlToken> lastStartTag = testObject.lookup("lastStartTag"s).map([](Serde::Value const& val) {
        return HtmlToken{HtmlToken::START_TAG, Symbol::from(val.asStr())};
    });

    Gc::Heap gc;
    Serde::Array actual;
    for (auto const& state : initialStates) {
        auto dom = gc.alloc<Dom::Document>(Ref::Url());

        HtmlParser parser{gc, dom};
        parser._lexer._lastStartTag = lastStartTag;

        auto collector = TokenCollectorSink(&parser);
        parser._lexer._sink = &collector;
        parser._lexer._state = stringToState(state.asStr());

        auto diags = Diag::Collector{};
        parser.write(input, diags);

        actual.pushBack(serializeTokens(collector.collected, doubleEscaped));
    }

    return Ok(Result{
        .reference = output,
        .actual = actual,
    });
}

} // namespace Html5LibTest::Tokenizer