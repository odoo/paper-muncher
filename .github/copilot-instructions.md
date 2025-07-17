### Pull request checklist

When submitting or reviewing a pull request, ensure the following criteria are met:

* Run tests and reftests. Add tests if applicable (fix for a bug, new feature, etc.).
* Don't be afraid of shaving yaks: if the code base is not ready for your features, address the systemic issue instead of working around it. Separate the changes into different PRs if suitable.
* Address technical debt, 'smelly code' identified or any code to be changed either during the PR discussion or with `NOTE`/`FIXME` comments.
* Code Quality & Readability
    * Use `or`/`and`/`not` instead of `||`/`&&`/`!`
    * Remove empty lines from the beginning of scopes.
    * Prefer early returns instead of `if` nesting/arrow code.
    * Make sure you removed dead code. Make sure you are not importing more than needed.
    * Constructor should be `O(1)`. Make them light (dependency injection)/do not build a lot inside a constructor. Prefer `static` builder methods if complex building logic is needed.
    * Since we don't 'enforce' private attrs/methods, make sure these are correctly prefixed with a `_` and such are not being called outside or the object scope.
* Comments related
    * If implementing something spec related, add a link to the spec. If the spec describes and algorithm/behavior and the code is a direct/close implementation of it, add snippets of the spec into the code.
    * Only add a `TODO` if you are actually planning to address it in a future PR. Otherwise, add a `NOTE` only if needed and do not 'apologize' for unimplemented features.
    * Make sure comments explain 'why' and not 'how'. If you need to explain 'how', code is not readable enough.
    * When writing comments, make sure each line is at most `80` characters. If a comments is 2 lines, make sure their width are close.
* Other
    * Format commit messages correctly: `(sub)module: First letter is capital, ends with period.`.
    * Make sure to remove debugging commands `logDebug`/`yap`.
    * If handling large external data/files, ensure performance considerations are met (eg. avoid 40k `if/else if`).
