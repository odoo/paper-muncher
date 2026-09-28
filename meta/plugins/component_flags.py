"""
Per-component compiler flags for CuteKit.

CuteKit merges every component's `tools` into the target-wide tools, so a
component cannot normally get its own flags. The ninja rule for a tool reads
`$<tool>flags` (e.g. `$ccflags`), and a ninja build statement can shadow that
variable. This plugin wraps the "obj" build node and, for components that
declare it, emits a per-object `ccflags` / `cxxflags` binding.

Component props:
    "ck-cc-extra":     [...]  appended after the target's cc flags
    "ck-cxx-extra":    [...]  appended after the target's cxx flags
    "ck-cc-override":  [...]  replaces the target's cc flags entirely
    "ck-cxx-override": [...]  replaces the target's cxx flags entirely
"""

from cutekit import builder

_origObj = builder._nodes["obj"][0]


def _asList(v) -> list[str]:
    return v.split() if isinstance(v, str) else list(v)


def _objWithFlags(scope, ruleId: str, src: str):
    props = scope.component.props
    override = props.get(f"ck-{ruleId}-override")
    extra = props.get(f"ck-{ruleId}-extra")

    if override is None and extra is None:
        return _origObj(scope, ruleId, src)

    var = f"{ruleId}flags"
    if override is not None:
        value = " ".join(_asList(override))
    else:
        # In ninja, a build-scope binding is evaluated in the file scope,
        # so `$ccflags` on the right-hand side is the target-wide value.
        value = f"${var} " + " ".join(_asList(extra))

    w = scope.writer
    origBuild = w.build

    def build(*args, variables=None, **kwargs):
        return origBuild(*args, variables={**(variables or {}), var: value}, **kwargs)

    w.build = build
    try:
        return _origObj(scope, ruleId, src)
    finally:
        del w.build  # drop the instance attribute, back to the class method


builder._nodes["obj"][0] = _objWithFlags