(function (hostPrint) {
    "use strict";

    // https://console.spec.whatwg.org/#generic-javascript-object-formatting
    function formatGenericJsObject(obj) {
        try {
            if (obj instanceof Error) return obj.stack ?? String(obj);
            if (typeof obj === "object" && obj !== null)
                try { return JSON.stringify(obj) ?? String(obj); } catch {}
            return String(obj);
        } catch {}
        try { return Object.prototype.toString.call(obj); } catch { return "[unprintable]"; }
    }

    // https://console.spec.whatwg.org/#formatter
    function Formatter(args) {
        // TODO
        return args;
    }

    // https://console.spec.whatwg.org/#printer
    function Printer(logLevel, args, options = undefined) {
        const parts = [];
        for (const arg of args)
            parts.push(typeof arg === "string" ? arg : formatGenericJsObject(arg));
        hostPrint(logLevel, parts.join(" "));
    }

    // https://console.spec.whatwg.org/#logger
    function Logger(logLevel, args) {
        if (args.length === 0)
            return;

        if (args.length === 1) {
            Printer(logLevel, [args[0]]);
            return;
        }

        Printer(logLevel, Formatter(args));
    }

    // For historical web-compatibility reasons,
    // the namespace object for console must have as its [[Prototype]] an empty object,
    // created as if by ObjectCreate(%ObjectPrototype%), instead of %ObjectPrototype%.
    const namespace = Object.create(Object.create(Object.prototype));

    const ops = {
        // https://console.spec.whatwg.org/#log
        log(...data) { Logger("log", data); },
        // https://console.spec.whatwg.org/#info
        info(...data) { Logger("info", data); },
        // https://console.spec.whatwg.org/#warn
        warn(...data) { Logger("warn", data); },
        // https://console.spec.whatwg.org/#error
        error(...data) { Logger("error", data); },
        // https://console.spec.whatwg.org/#debug
        debug(...data) { Logger("debug", data); },
    };

    for (const [k, f] of Object.entries(ops)) {
        Object.defineProperty(namespace, k, {
            value: f,
            writable: true,
            enumerable: true,
            configurable: true,
        });
    }

    Object.defineProperty(globalThis, "console", {
        value: namespace,
        writable: true,
        enumerable: false,
        configurable: true,
    });
});