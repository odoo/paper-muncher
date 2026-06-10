# Reftest File Format

Reftests are `.xhtml` files in `tests/`. Each file contains one or more tests. Each test renders a **reference** and compares every following case against it, pixel-for-pixel.

## Minimal example

```xml
<tests>
    <test name="my-first-test">
        <rendering>
            <div style="width: 100px; height: 100px; background: red" />
        </rendering>
        <rendering>
            <div style="width: 100px; height: 100px; background: red" />
        </rendering>
    </test>
</tests>
```

## Structure

```
<tests>                      root element (required)
└── <test …props>            one test = one reference + N cases
    ├── <container>          optional custom document wrapper
    │   └── <html>…<slot/>…  case content replaces <slot/>
    ├── <rendering …props>   FIRST case = reference image
    ├── <rendering …props>   must match the reference → pass
    └── <error …props>       must NOT match the reference → pass
```

Rules:

- The root element must be `<tests>`; its children must be `<test>`.
- **The first case of a `<test>` is the reference.** It is rendered but not itself checked.
- `<rendering>` passes if its output is byte-identical to the reference image.
- `<error>` passes if its output **differs** from the reference image.
- Comparison is exact (`bytes ==`); any pixel difference fails a `<rendering>`.

## `<test>` properties

| Attribute | Values                              | Default   | Effect                              |
| --------- | ----------------------------------- |-----------| ----------------------------------- |
| `name`    | string                              | /         | Human-readable test name            |
| `width`   | CSS length                          | `"200px"` | Viewport/page width (`--width`)     |
| `height`  | CSS length                          | `"200px"` | Viewport/page height (`--height`)   |
| `flow`    | `auto` \| `paginate` \| `continuous`| unset     | Layout flow mode (`--flow`)         |
| `margins` | CSS margin value                    | unset     | Page margins (`--margins`)          |
| `skip`    | `"true"`                            | unset     | Skip every case in this test        |

## `<rendering>` / `<error>` properties

| Attribute | Values   | Default | Effect                                 |
| --------- | -------- |---------| -------------------------------------- |
| `help`    | string   | /       | Note/explanation shown in the report   |
| `skip`    | `"true"` | unset   | Skip this case only                    |

Skipped cases are not run unless the suite is invoked with `--run-skipped`.

## Containers

By default, case content is injected into the `<body>` of:

```xml
<html xmlns="http://www.w3.org/1999/xhtml"><body><slot /></body></html>
```

To control the surrounding document (e.g. add `<style>`, test `@page` rules), provide a `<container>` as the first child of `<test>`:

```xml
<test name="styled-test">
    <container>
        <html xmlns="http://www.w3.org/1999/xhtml">
            <head>
                <style>
                    div { border: 1px solid black; }
                </style>
            </head>
            <body>
                <slot />
            </body>
        </html>
    </container>
    <rendering>
        <div>reference</div>
    </rendering>
    <rendering>
        <div>should look the same</div>
    </rendering>
</test>
```

Container rules:

- The container's root must be an `<html>` element.
- It must contain exactly one `<slot/>`; each case's children are inserted in its place, in order.
- Injected case content inherits the container's XML namespace automatically.
- One container applies to **all** cases of its test, including the reference.

## Pagination example

```xml
<test name="page-break" flow="paginate" width="210mm" height="297mm" margins="10mm">
    <rendering>
        <div style="break-after: page">page 1</div>
        <div>page 2</div>
    </rendering>
    <error help="without the break, content stays on one page">
        <div>page 1</div>
        <div>page 2</div>
    </error>
</test>
```
