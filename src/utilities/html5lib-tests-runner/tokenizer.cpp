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

    TokenCollectorSink(HtmlSink* sink) : sink(sink) {
    }

    void accept(HtmlToken& token, Diag::Collector& diags) override {
        collected.pushBack(token);
        sink->accept(token, diags);
    }
};

String unescape(Str str) {
    Io::SScan s = {str};
    Io::StringWriter sw;

    while (not s.ended()) {
        if (s.skip('\\')) {
            if (s.skip('u')) {
                auto u = Io::atou(s, {.base = 16});
                if (u) {
                    sw.append(*u);
                    continue;
                }
            }
        }

        sw.append(s.next());
    }

    return sw.take();
}

static String escape(Str s) {
    Io::StringWriter sw;
    auto emit = Io::Emit{sw};

    for (auto c : iterRunes(s)) {
        if (c < 0x20 || c > 0x7E) {
            emit("\\u{04X}", c);
        } else {
            emit(c);
        }
    }

    return sw.take();
}

HtmlLexer::State stringToState(Str s) {
    if (s == "Data state") {
        return HtmlLexer::State::DATA;
    } else if (s == "PLAINTEXT state") {
        return HtmlLexer::State::PLAINTEXT;
    } else if (s == "RCDATA state") {
        return HtmlLexer::State::RCDATA;
    } else if (s == "RAWTEXT state") {
        return HtmlLexer::State::RAWTEXT;
    } else if (s == "Script data state") {
        return HtmlLexer::State::SCRIPT_DATA;
    } else if (s == "CDATA section state") {
        return HtmlLexer::State::CDATA_SECTION;
    } else {
        notImplemented();
    }
}

Serde::Array serializeTokens(Slice<HtmlToken> const& tokens, bool doubleEscaped) {
    Serde::Array serializedTokens;
    StringBuilder builder;

    auto flushCharacterBuffer = [&]() {
        if (builder.len() > 0) {
            Serde::Array charToken;
            charToken.pushBack("Character"s);

            if (doubleEscaped) {
                charToken.pushBack(escape(builder.take()));
            } else {
                charToken.pushBack(builder.take());
            }

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

        if (t.type == HtmlToken::DOCTYPE) {
            serializedToken.pushBack("DOCTYPE"s);

            if (doubleEscaped) {
                serializedToken.pushBack(escape(t.name.str()));
            } else {
                serializedToken.pushBack(t.name.str());
            }

            if (t.publicIdent.len() > 0) {
                if (doubleEscaped) {
                    serializedToken.pushBack(escape(t.publicIdent));
                } else {
                    serializedToken.pushBack(t.publicIdent);
                }
            } else {
                serializedToken.pushBack(NONE);
            }

            if (t.systemIdent.len() > 0) {
                if (doubleEscaped) {
                    serializedToken.pushBack(escape(t.systemIdent));
                } else {
                    serializedToken.pushBack(t.systemIdent);
                }
            } else {
                serializedToken.pushBack(NONE);
            }

            serializedToken.pushBack(not t.forceQuirks);

        } else if (t.type == HtmlToken::START_TAG) {
            serializedToken.pushBack("StartTag"s);
            if (doubleEscaped) {
                serializedToken.pushBack(escape(t.name.str()));
            } else {
                serializedToken.pushBack(t.name.str());
            }

            Serde::Object attrs;
            for (auto const& attr : t.attrs) {
                if (doubleEscaped) {
                    attrs.put(escape(attr.name.str()), escape(attr.value.str()));
                } else {
                    attrs.put(attr.name.str(), attr.value.str());
                }
            }
            serializedToken.pushBack(std::move(attrs));

            if (t.selfClosing) {
                serializedToken.pushBack(true);
            }

        } else if (t.type == HtmlToken::END_TAG) {
            serializedToken.pushBack("EndTag"s);
            if (doubleEscaped) {
                serializedToken.pushBack(escape(t.name.str()));
            } else {
                serializedToken.pushBack(t.name.str());
            }

        } else if (t.type == HtmlToken::COMMENT) {
            serializedToken.pushBack("Comment"s);
            if (doubleEscaped) {
                serializedToken.pushBack(escape(t.data.str()));
            } else {
                serializedToken.pushBack(t.data.str());
            }

        } else {
            continue;
        }

        serializedTokens.pushBack(std::move(serializedToken));
    }

    flushCharacterBuffer();

    return serializedTokens;
}

// https://github.com/html5lib/html5lib-tests/blob/master/tokenizer/README.md
export Res<Result> run(Str inputStr) {
    auto testObject = try$(Json::parse(inputStr)).asObject();

    auto input = testObject.get("input"s).asStr();
    auto output = testObject.get("output"s).asArray();

    auto doubleEscaped = testObject.getOrDefault("doubleEscaped"s, false);

    if (doubleEscaped) {
        input = unescape(input);
    }

    auto initialStates = testObject.getOrDefault("initialStates"s, Serde::Array{"Data state"s}).asArray();

    Opt<HtmlToken> lastStartTag = testObject.tryGet("lastStartTag"s).map([](Serde::Value const& val) {
        return HtmlToken{
            .name = Symbol::from(val.asStr()),
        };
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