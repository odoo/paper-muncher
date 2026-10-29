#include <karm/test>

import Karm.Diag;
import Karm.Ref;
import Vaev.Engine;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Ref::Literals;

namespace Vaev::Style::Tests {

static StyleSheet _parseSheet(Str css) {
    RegisteredPropertySet registry = defaultRegistry();
    Io::SScan s{css};
    auto diags = Diag::Collector::ignore();
    return StyleSheet::parse(registry, s, diags, ""_url);
}

test$("layer-rule-parse-statement") {
    auto sheet = _parseSheet("@layer a, b.c ,d;");
    assertEq$(sheet.rules.len(), 1uz);

    auto rule = sheet.rules[0].is<LayerStatementRule>();
    assert$(rule.has());
    assertEq$(rule->names.len(), 3uz);
    assertEq$(rule->names[0], LayerName{"a"_sym});
    assertEq$(rule->names[1], (LayerName{"b"_sym, "c"_sym}));
    assertEq$(rule->names[2], LayerName{"d"_sym});

    return Ok();
}

test$("layer-rule-parse-block") {
    auto sheet = _parseSheet("@layer a.b { div { color: red; } } @layer { p { color: blue; } }");
    assertEq$(sheet.rules.len(), 2uz);

    auto named = sheet.rules[0].is<LayerBlockRule>();
    assert$(named.has());
    assert$(named->name == (LayerName{"a"_sym, "b"_sym}));
    assertEq$(named->rules.len(), 1uz);
    assert$(named->rules[0].is<StyleRule>().has());

    auto anonymous = sheet.rules[1].is<LayerBlockRule>();
    assert$(anonymous.has());
    assert$(anonymous->name == NONE);
    assertEq$(anonymous->rules.len(), 1uz);

    return Ok();
}

test$("layer-rule-parse-invalid") {
    for (auto css : {"@layer a b;"s, "@layer a. b;"s, "@layer a,;"s, "@layer initial;"s, "@layer a, b { }"s, "@layer revert-layer { }"s}) {
        auto sheet = _parseSheet(css);
        assertEq$(sheet.rules.len(), 1uz);
        auto rule = sheet.rules[0].is<LayerStatementRule>();
        assert$(rule.has());
        assertEq$(rule->names.len(), 0uz);
    }

    return Ok();
}

test$("cascade-layers-ordering") {
    CascadeLayers layers;
    auto root = layers.root(Origin::AUTHOR);

    auto b = layers.declare(root, {"b"_sym});
    auto a = layers.declare(root, {"a"_sym});
    auto aInner = layers.declare(root, {"a"_sym, "inner"_sym});
    auto anon1 = layers.declareAnonymous(root);
    auto anon2 = layers.declareAnonymous(root);

    assertEq$(layers.declare(root, {"b"_sym}).value(), b.value());
    assertEq$(layers.declare(a, {"inner"_sym}).value(), aInner.value());

    assertNe$(anon1.value(), anon2.value());

    layers.finalize();

    assertLt$(layers.rank(b), layers.rank(aInner));
    assertLt$(layers.rank(aInner), layers.rank(a));
    assertLt$(layers.rank(a), layers.rank(anon1));
    assertLt$(layers.rank(anon1), layers.rank(anon2));
    assertLt$(layers.rank(anon2), layers.rank(root));

    return Ok();
}

test$("cascade-layers-per-origin") {
    CascadeLayers layers;
    auto ua = layers.declare(layers.root(Origin::USER_AGENT), {"a"_sym});
    auto author = layers.declare(layers.root(Origin::AUTHOR), {"a"_sym});
    assertNe$(ua.value(), author.value());
    return Ok();
}

} // namespace Vaev::Style::Tests
