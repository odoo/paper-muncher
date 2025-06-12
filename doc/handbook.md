# Paper Muncher Developer Handbook

## Welcome to the team!

We're genuinely excited to have you join us. Before you dive in, please take some time to read through our [Code of Conduct](https://www.contributor-covenant.org/version/2/1/code_of_conduct/)

## About the Project

Paper Muncher is the fruit of a long search for a wkhtmltopdf replacement (See our [FAQ](./faq.md) to understand why other options like Weasyprint and Chrome were rejected). It's based on a toy browser engine that @sleepy-monax (a.k.a clvb) wrote on their freetime now called Vaev. It's composed of:
 - Markup parser `vaev-dom` responsible for parsing HTML and XML into a DOM.
 - Style engine `vaev-style` responsible for parsing CSS into stylesheet objects, and computing styles.
 - Layout engine `vaev-layout` takes the computed style and DOM to build a fragment tree. This fragment tree is then laid out following the different formatting options offered by CSS (flex, grid, table, block and inline)
 - And a driver `vaev-driver` that ties all the other components together

(See [the architecture diagram](./diagrams.tldr) for a visual representation of how these components interact)

## Team Values

As a team we believe that building exceptional software requires strong opinions, strong vision, openness, and respect. We keep our process as lean and flexible as possible. We are here to have fun, grow, and build an exceptional product.

## Essential Tools

* Any text editor or IDE that support clangd (e.g., VSCode, CLion, etc.)
* [Clangd for VSCode](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)
* [tldraw for VSCode](https://marketplace.visualstudio.com/items?itemName=tldraw-org.tldraw-vscode)

## Essential Commands

* `./ck run vaev-browser -- <input document>`: Start the development environment.
* `./ck run --debug vaev-browser -- <input document>`: For debugging.
* `./ck run --profile vaev-browser -- <input document>`: For profiling (requires `perf` and `speedscope` to be installed).
* `./ck run --release vaev-browser -- <input document>`: Build the release version.
* `./ck test`: Run all tests.
* `./ck clean`: Clean the build directory.

## Essential Editor Configuration

* To maintain a consistent code style, we use clang-format to format our code. You can configure your editor to use it on save or use the `./ck fmt -f` script.

## Essential Resources

* [Cppreference](https://en.cppreference.com/w/)
* [CSS Latest Snapshot](https://www.w3.org/TR/CSS)
* [HTML Living Standard](https://html.spec.whatwg.org/multipage/)
* [SVG 2](https://www.w3.org/TR/SVG2/)
* [MathML 3](https://www.w3.org/TR/MathML3/)
* [XML 1.0](https://www.w3.org/TR/REC-xml/)

## Our C++ Style

We've adopted a specific C++ style to help us avoid common pitfalls and write maintainable code.

* **Modern C++23:** We use the latest features like concepts and coroutines.
* **Concepts over template metaprogramming:** For better readability.
* **Custom containers:** We use `Set<T>`, `Map<K, V>`, `Vec<T>`, `Opt<T>`, `Union<Ts...>`, `Tuple<Ts...>`, etc. instead of `std::` equivalents. They avoid most UB and pitfalls of the standard library (e.g., `std::vector<bool>`).
* **No exceptions:** We use `Res<T>` and `try$()` for error handling to avoid exception complexities (Did you know that destructors can throw exceptions?).
* **No RTTI:** We use `Union<Ts...>` instead for better control over memory layout and performance.
* **Structs over classes:** Used by default since there's little practical difference in C++.
* **No private/public:** Private variables are prefixed with `_` to indicate visibility. This simplifies testing.
* **Short names for common things:** Frequently used types/functions have concise names (e.g., `Io::Scan` instead of `Io::PlainTextScanner`).
* See [karmism](https://github.com/skift-org/karm/blob/main/doc/karmism.md) for more details.

## Testing

* **Focus on tricky parts:** Test complex or error-prone code thoroughly.
* **Test changing code:** Ensure frequently modified code remains correct.
* Most layout and parsing code should be validated with W3C tests.

## Source Control & Code Review

* **Commit often:** Keep commits small and focused.
* **Use branches:** Name branches like `<your-ngram>/<your-feature>`.
* **Merge small changes quickly:** Bugfixes can be merged directly into `main`.
* **Seek code reviews:** Request reviews from any team member. Merge after approval and CI passes.

## Remote Work

We are at GR2 on Mondays, Wednesdays, and Thursdays, and remote otherwise.

## Remember

These are guidelines, not rigid rules. Use your best judgment and ask questions when unsure.
