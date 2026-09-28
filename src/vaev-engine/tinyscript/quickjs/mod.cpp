module;

#include <quickjs.h>

module Vaev.Engine;

import :tinyscript.runtime;

namespace Vaev::TinyScript::_Embed {

struct QuickJsAgent : Agent {
    JSRuntime* _rt;

    QuickJsAgent(JSRuntime* rt) : _rt(rt) {}

    static Rc<Agent> create() {
        auto* rt = JS_NewRuntime();

        if (rt == nullptr)
            panic("failed to create js agent");

        return makeRc<QuickJsAgent>(rt);
    }

    ~QuickJsAgent() override {
        JS_FreeRuntime(_rt);
    }
};

static Rc<QuickJsAgent> _assertQuickJsAgent(Rc<Agent> agent) {
    if (auto it = agent.cast<QuickJsAgent>()) {
        return *it;
    }
    panic("expected quickjs agent");
}

static JSValue _print(JSContext* ctx, JSValueConst, int argc, JSValueConst* argv) {
    size_t len = 0;
    char const* buf = argc ? JS_ToCStringLen(ctx, &len, argv[0]) : "";
    auto str = Str{buf, len};

    if (not str) {
        JS_FreeValue(ctx, JS_GetException(ctx)); // don't let print throw
        logInfo("[unprintable]");
    } else {
        logInfo("{}", str);
        if (argc)
            JS_FreeCString(ctx, buf);
    }
    return JS_UNDEFINED;
}

static void _printException(JSContext* ctx) {
    JSValue exc = JS_GetException(ctx);

    usize len = 0;
    char const* buf = JS_ToCStringLen(ctx, &len, exc);

    if (buf) {
        logError("{}", Str{buf, len});
        JS_FreeCString(ctx, buf); // only after the last use of the view
    } else {
        // toString() threw.
        JS_FreeValue(ctx, JS_GetException(ctx));
        logError("[unprintable]");
    }

    // Only Error objects carry a stack; `throw "str"` or `throw 42` don't.
    if (JS_IsError(exc)) {
        JSValue stack = JS_GetPropertyStr(ctx, exc, "stack");

        if (JS_IsException(stack)) {
            // throwing "stack" getter
            JS_FreeValue(ctx, JS_GetException(ctx));
        } else if (JS_IsString(stack)) {
            usize stackLen = 0;
            char const* stackBuf = JS_ToCStringLen(ctx, &stackLen, stack);
            if (stackBuf) {
                if (stackLen and stackBuf[stackLen - 1] == '\n')
                    stackLen--;
                if (stackLen)
                    logError("{}", Str{stackBuf, stackLen});
                JS_FreeCString(ctx, stackBuf);
            } else {
                JS_FreeValue(ctx, JS_GetException(ctx));
            }
        }

        JS_FreeValue(ctx, stack);
    }

    JS_FreeValue(ctx, exc);
}

struct QuickJsRealm : Realm {
    Rc<QuickJsAgent> _agent;
    JSContext* _ctx;

    QuickJsRealm(Rc<QuickJsAgent> _agent, JSContext* ctx)
        : _agent(std::move(_agent)), _ctx(ctx) {}

    static Rc<Realm> create(Rc<QuickJsAgent> agent) {
        auto* ctx = JS_NewContext(agent->_rt);

        if (ctx == nullptr)
            panic("failed to create js realm");

        JSValue global = JS_GetGlobalObject(ctx);
        JS_SetPropertyStr(ctx, global, "print", JS_NewCFunction(ctx, _print, "print", 1));
        JS_FreeValue(ctx, global);

        return makeRc<QuickJsRealm>(agent, ctx);
    }

    void eval(Str src) override {
        auto r = JS_Eval(_ctx, src.buf(), src.len(), "<eval>", JS_EVAL_TYPE_GLOBAL);
        if (JS_IsException(r)) {
            _printException(_ctx);
        }
        JS_FreeValue(_ctx, r);
    }

    ~QuickJsRealm() override {
        JS_FreeContext(_ctx);
    }
};

Rc<Agent> createAgent() {
    return QuickJsAgent::create();
}

Rc<Realm> createRealm(Rc<Agent> agent) {
    return QuickJsRealm::create(_assertQuickJsAgent(agent));
}

}
