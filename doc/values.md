# CSS Value System

How CSS values are represented, where each type lives, and how values move
between stages. The rules follow the specs: when in doubt, read the spec's
definition table and mirror it.

## The model

Each stage exposes a complete vocabulary. A type is defined once, at the
earliest stage that needs it; later stages re-export it when it is identical,
or define their own plain type when it differs.

| Stage     | Namespace      | Folder          | Example                                     |
| --------- | -------------- | --------------- | ------------------------------------------- |
| Specified | `Vaev::Css`    | `css/values/`   | `Css::LengthPercentage` (may contain calc)  |
| Computed  | `Vaev::Style`  | `style/values/` | `Style::PixelPercentage` (absolute px, unresolved %) |
| Used      | `Vaev::Layout` | `layout/`       | `Layout::Au`                                |

The namespace carries the stage: no `Specified*` / `Computed*` prefixes.

## Layers

| Location             | Namespace         | Contains                                                         |
| -------------------- | ----------------- | ---------------------------------------------------------------- |
| `css/values/`        | `Vaev::Css`       | The language: keywords, idents, units, calculation trees and their type checking, specified types, all value parsers and serializers |
| `style/values/`      | `Vaev::Style`     | Computed types that differ from specified, re-exports of those that don't, `toComputed`/`toSpecified`, calc simplification, relative-unit resolution |
| `style/computed.cpp` | `Vaev::Style`     | `ComputedValues`, its groupings, computed types of properties    |
| `props/`             | `Vaev::Style`     | Property definitions and grammars: the bridge between stages     |
| `layout/`, `paint/`  | `Layout`, `Paint` | Used values, `Au` and its geometry aliases                       |

Hard rules:

- `Css` never names `Style`.
- `style/values/` never parses: value parsers live in `css/values/`.
  (Rules, declarations, descriptors and properties are parsed by their own
  definitions in `style/` and `props/`.)
- `layout/` and `paint/` never name `Css` nor import `:css.*`; they speak
  `Style` (and `Au`).
- `props/` is the only code that names both stages.
- Only layout knows `Au`. Style computes to `Pixel`, layout rounds to `Au`.

## Where does a type go?

| The spec defines…                                                | Goes to |
| ---------------------------------------------------------------- | ------- |
| A **named production** (`<length>`, `<display-outside>`, `<counter-style>`) | `css/values/`. In `style/values/`: a computed type if computation changes it, otherwise a re-export (`using Display = Css::Display;`). |
| A production whose **leaves** change (`<basic-shape>` holds lengths) | A plain struct in each stage with the same fields (`Css::Circle` with `Css::LengthPercentage`, `Style::Circle` with `Style::PixelPercentage`), linked by `toComputed`/`toSpecified`. No stage-parameterized templates. |
| A **property's Value line** (its own grammar, shorthands included) | The property itself: `using Value = <grammar>;`. |
| A **property's Computed value** rule                             | The property's `apply()` (and back to specified in `load()`). |
| The **computed type** a property stores in `ComputedValues`      | `style/computed.cpp`, "Computed value types". If specified equals computed (keyword unions), the property reuses this type as its grammar. |
| A **descriptor** (`@font-face`) or **media feature** grammar     | Its definitions: `style/fonts.cpp`, `style/media.cpp`. |
| Something that isn't a CSS value (an SVG element kind, …)        | The code that uses it. |

### Re-exports

Identical types are re-exported, never redefined, so each stage names its own
vocabulary:

```cpp
// style/values/display.cpp
export using Display = Css::Display;

// style/values/keywords.cpp
export namespace Keywords = Css::Keywords;

// style/values/calc.cpp: a computed math function is still a math function
export using Css::Calc;
```

If a re-exported type later needs a distinct computed form, replace the alias
with a real type; consumers already name `Style::X` and don't change.

Only **values** are re-exported, never **syntax**. Grammar combinators
(`a || b`, `a && b`, `#`, …) exist only in `Css`: they parse into plain
structure (`Opt`, `Vec`, structs), and that structure is what properties and
`Style` hold.

### Property grammars

A property's grammar is its nested `Value`, written as a type that mirrors the
spec's Value line. Parsing and serialization come from the generic parsers of
the types it is built from (`Union`, `Vec`, `Opt`, …):

```cpp
// https://www.w3.org/TR/css-sizing-3/#propdef-width
export struct WidthProperty : Property {
    using Value = Union<Keywords::Auto, Css::LengthPercentage, Keywords::Stretch, …>;
    …
};
```

- Inside a grammar, qualify every CSS term with `Css::`, as the spec brackets
  it (`<line-width> || <color>` → `Css::AnyOrder<Css::LineWidth, Css::Color>`).
  Only Karm structure (`Union`, `Opt`, `Vec`) and `Keywords::` stay bare.
- Properties with the same grammar alias it: `using Value = WidthProperty::Value;`.
  Shorthands refer to `XProperty::Value`. This is the spec's `<'width'>`.
- `Registration::parse` is `parseValue<Value>(c)` plus genuinely
  property-specific handling.
- A grammar the generic parsers can't express yet is parsed by the property's
  own code:
  - used by one property: directly in its `Registration::parse`;
  - shared: a `Value` struct with `static parse` (and `static serialize`),
    which `parseValue<Value>` picks up.

  These go away as grammar wrappers (multipliers, `||`, `&&`, ranges) are added.
- A property may use its computed type as its grammar only when every
  component computes to itself (keywords, idents, strings). Anything with
  `<integer>`, `<length>`, `<image>`, `url()`… gets a specified `Value`.
- Types in `css/values/` carry their parser as `static parse` /
  `static serialize` members. `ValueParser<T>` / `ValueSerializer<T>`
  specializations are for types that can't carry members: aliases
  (`Union<…>`) and types we don't own.

## Files

- One file per spec module, per layer, named after the spec without prefix
  or level: css-backgrounds-3 → `backgrounds.cpp`, css-counter-styles-3 →
  `counter-styles.cpp`.
- The same name in every layer that has content for that spec, re-exports
  included; a layer without content has no file.
- Large specs are split by section (css-values: `unit-value`, `calc`,
  `numeric`). Infrastructure: `keywords`, `primitives`, `base`, `generic`.
- Partitions follow the path: `css/values/counter-styles.cpp` →
  `:css.values.counterStyles`.

## Types

**Numbers.** Use the calc-capable types (`Css::Length`, `Css::Percentage`,
`Css::LengthPercentage`, …) wherever the spec allows math functions, which is
almost everywhere. `Css::Pure*` (no calc) is reserved for inputs outside CSS
with no context, such as command-line options.

**Computed numbers** use the canonical unit of their type:

| Spec type       | Computed            | Literal  |
| --------------- | ------------------- | -------- |
| `<integer>`     | `Style::Integer`    |          |
| `<number>`      | `f64`               |          |
| `<length>`      | `Style::Pixel`      | `_px`    |
| `<percentage>`  | `Style::Percent`    |          |
| `<angle>`       | `Style::Deg`        | `_deg`   |
| `<resolution>`  | `Style::Dppx`       | `_dppx`  |
| `<length-percentage>` | `Style::PixelPercentage` (`Pixel`, `Percent`, `px + %`, or an unresolved `Calc`) | |

Literals live in `Vaev::Literals`. Convert to other units where they are
used, e.g. `deg * Math::DEG2RAD` in paint.

**Keywords.** Keyword-valued properties are `Union<Keywords::…>`, not enums.
Switch over them with `visit`. Inside `Style` (props included) write
`Keywords::Auto`; in layout and paint, `Style::Keywords::Auto`.

**Naming.** Follow the spec's term. On a clash, the production keeps the
plain name and the other type takes the spec's term for it (`<position>`
→ `Style::Position`, the `position` property → `Style::PositionScheme`;
`<counter-style>` → `CounterStyle`, an `@counter-style` rule's result →
`CounterStyleDefinition`).

## Conversions

| Direction             | Function                       | Lives in |
| --------------------- | ------------------------------ | -------- |
| specified → computed  | `toComputed(value, ctx)`       | `style/values/`, or the property for property rules |
| computed → specified  | `toSpecified(value)` (serialization goes through `load()` then this) | same |
| computed → used       | `resolve(value, basis)`        | `Layout::resolve` for lengths and percentages; `Style::resolve` for `currentcolor` |

- **Crossing several stages:** `toX` names the stage you land in. A helper
  that skips a stage is exactly the composition of the stage functions, e.g.
  `toUsed(color, currentColor)` = `resolve(toComputed(color, currentColor),
  currentColor)` (the `color` property, `outline-color`).
- **Unions convert automatically** (`style/values/generic.cpp`). Don't write
  `visit` forwarders; write an explicit overload only when there is real
  logic. It is more specialized, so it wins.
- **Re-exported types** need no conversion; keywords convert to themselves.
- **Calc** is never simplified at parse time (a Vaev choice; the spec
  simplifies parsed values). `Css` only builds and type-checks calculation
  trees; `Style::simplify` (`style/values/calc.cpp`) runs at compute time,
  with a context for relative units, and at used time, with a percentage
  basis. The result is in the canonical unit, so check the unit and read
  `value()`, no conversion needed. `Pure*` values aren't simplified: name the
  unit, `.to(Unit::DEG)`.
- **Ranges** (`[0,∞]`, …): clamp calc results at compute time.
- **Lookup:** `Style` conversions are found through the context argument
  (a `Style` type). A context from another namespace only sees overloads the
  generic imports.
- **Relative units** are resolved by `Style::toCanonical(unitValue, ctx)`
  (`style/values/unit-value.cpp`), with a `LengthContext` for `em`, `vw`,
  `vi`, … or `NONE` for absolute units only. Contexts
  (`RelativeLengthContextData`, `LengthContext`) live in `Style`.

## Imports

- Import exactly the partitions you use; never rely on a transitive import.
- Aggregates (`:css.values`, `:style.values`) are for consumers outside the
  value layers.

## Adding a property

1. Find its grammar in the spec. Named productions you need go in
   `css/values/`, with a computed type or a re-export in `style/values/`;
   the property's own grammar is its `using Value`.
2. Add its computed type to `style/computed.cpp` unless a production's type
   fits.
3. Implement `parse`, `apply` (the spec's Computed value), `load` (back to
   specified via `toSpecified`).
4. Run the WPT parsing tests; move newly passing assertions out of the
   `-xfail` test.
