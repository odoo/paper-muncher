# Paper Muncher Developer Handbook

## Welcome to the team!

We're genuinely excited to have you join us. Before you dive in, please take some time to read through our [Code of Conduct](https://www.contributor-covenant.org/version/2/1/code_of_conduct/)

## About the Project

Paper Muncher is the fruit of a long search for a wkhtmltopdf replacement (See our [FAQ](faq.md) to understand why other options like Weasyprint and Chrome where rejected). It's based of a toy browser engine that @sleepy-monax (a.k.a nivb) wrote on their freetime now called Vaev. Its composed of a:
 - Markup parser `vaev-markup` responsible for parsing html and xml into a DOM.
 - Style engine `vaev-style` responsible for parsing CSS into stylesheet object, and compute style.
 - Layout engine `vaev-layout` takes the computed style and DOM and build a fragment tree, this fragment tree is then layout following the different formating option offered by CSS (flex, grid, table, block and inline)
 - Paint engine `vaev-paint`, takes the layout tree and build a paint tree, this paint tree is then ready to be displayed on the screen or converted to a PDF file.
 - And a driver `vaev-driver` that tie all the other component together

(See [the architecture diagram](../src/web/diagrams.tldr) for a visual representatio of how these component interacts)

## Team Values

As a team we believe that building exceptional software requires strongs opinions, strong vision, openness, and respect. We keep our process as lean and flexible as possible. We are here to have fun, grow, and build an exceptional product.

## Essential Tools

* [VSCode](https://code.visualstudio.com/) (zed or neovim works too)
* [Clangd for VSCode](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)
* [tldraw for VSCode](https://marketplace.visualstudio.com/items?itemName=tldraw-org.tldraw-vscode)

## Essential Commands

* `./ck builder run paper-muncher inspector <input document>`:  Start the development environment.
* `./ck builder run --debug paper-muncher inspector <input document>`:  For debugging.
* `./ck builder run --profile paper-muncher inspector <input document>`:  For profiling (requires `perf` and `speedscope` to be installed).
* `./ck builder run --mixins=release paper-muncher inspector <input document>` Build the release version.
* `./ck builder test`  Run all tests.
* `./ck builder clean`  Clean the build directory.

## Essential editor configuration

* In a way to have a consistent code style, we use clang-format to format our code, you can configure your editor to use it on save or use the `./meta/scripts/style-format.sh` script.

## Essential Ressources

* [Cppreference](https://en.cppreference.com/w/)
* [CSS Snapshot 2023](https://www.w3.org/TR/CSS)
* [HTML Living Standard](https://html.spec.whatwg.org/multipage/)
* [SVG 2](https://www.w3.org/TR/SVG2/)
* [MathML 3](https://www.w3.org/TR/MathML3/)
* [XML 1.0](https://www.w3.org/TR/REC-xml/)

## Our C++ Style

We've adopted a specific C++ style to help us avoid common pitfalls and write maintainable code.

* **Modern C++23:**  We use the latest features like concepts and coroutines.
* **Concepts over template metaprogramming:**  For better readability.
* **Custom containers:**  We use `Set<T>`, `Map<K, V>`, `Vec<T>`, `Opt<T>`, `Union<Ts...>`, `Tuple<Ts...>`, etc. instead of `std::` They avoid most UB and pitfall of the std (e.g. std::vector\<bool>)
* **No exceptions:**  We use `Res<T>` and `try$()` for error handling to avoid the complexities and potential issues with exceptions in C++ (e.g., what should happen if a constructor throws an exception? What if a destructor throws an exception? What if a destructor is called during stack unwinding? etc.)
* **No rtti:**  We use `Union<Ts...>` instead of rtti for better control over memory layout and performance.
* **Structs over classes:**  We've chosen to use `struct` by default, as there's little practical difference in C++ and a choice has to be made.
* **No private/public:**  We prefix private variables with an underscore to indicate they're private and avoid the need for `private:` and `public:` sections. This makes testing easier and removes the need for hacks like `#define private public`.
* **Short names for common things:**  Frequently used types and functions have shorter names for better readability, e.g., `Io::PlainTextScanner` becomes `Io::Scan`.

## Testing

* **Focus on tricky parts:**  Test code that's complex or error-prone.
* **Test things that change often:**  Ensure that frequently modified code remains correct.
* Most layout and parsing code should be tested with W3C tests

## Source Control & Code Review

* **Commit often:**  Keep your commits small and focused.
* **Use branches:**  Name your branches like `<your-trigram>-<your-feature>`.
* **Merge small changes quickly:**  Bugfixes can be merged directly into `main`.
* **Seek code reviews:** Ask anyone for a code review when you're ready. Once you both agree the code is good and that the CI is green, merge it into `main`.

## Remote Work

We are at GR2 on Wednesdays and Thursdays, and remote the rest of the week.

## Remember

These are guidelines, not rigid rules. Use your best judgment and don't hesitate to ask if you have questions.
