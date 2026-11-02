# Upcomming Release

## Changes

- Removed Herobrine.

[GitHub Link](https://github.com/odoo/paper-muncher/)

# 🦣 Paper Muncher v0.10.0

## Changes

 - Added per-document headers and footers in batch mode [#305](https://github.com/odoo/paper-muncher/pull/305)
 - Sped up batch rendering by sharing parsed stylesheets between documents [#346](https://github.com/odoo/paper-muncher/pull/346)
 - Added support for WebP, AVIF, JPEG XL and TIFF images [karm@32b4595](https://codeberg.org/skift/karm/commit/32b4595638bcc48b6237c59094163a4b06eb64f4)
 - Replaced the default fonts with a more web friendly set [karm@a1df473](https://codeberg.org/skift/karm/commit/a1df473bd1c512251194aaaa5e95f8a3452ce2c8)
 - Replaced the VGA fallback font with a tofu font [karm@5c38cf5](https://codeberg.org/skift/karm/commit/5c38cf5c010b2717c27dbbdfbb26fe451eb8fbdd)
 - Fixed font family matching for names with non-alphabetic characters [karm@d432809](https://codeberg.org/skift/karm/commit/d432809aa382f7bde40ea7985631fa675abaabeb)
 - Added support for `<template>` elements and fixed a few HTML parser edge cases [#336](https://github.com/odoo/paper-muncher/pull/336)
 - Fixed comment handling in the CSS lexer [#325](https://github.com/odoo/paper-muncher/pull/325)
 - Fixed a crash with ruby boxes [#309](https://github.com/odoo/paper-muncher/pull/309)
 - Fixed the sandbox build on non-Linux targets [9478416](https://github.com/odoo/paper-muncher/commit/9478416b8802bc7c0df5a8c4f7c2bfad17d67b5d)
 - Reworked the inspector with print preview, fragment inspection and color previews [#328](https://github.com/odoo/paper-muncher/pull/328) [#330](https://github.com/odoo/paper-muncher/pull/330) [#331](https://github.com/odoo/paper-muncher/pull/331)
 - Added initial support for cascade layers (`@layer`) [219edb4](https://github.com/odoo/paper-muncher/commit/219edb4748b3f5521307f125b17874401f3eb951)
 - Added CSS serialization for stylesheets, properties and colors [#326](https://github.com/odoo/paper-muncher/pull/326) [#327](https://github.com/odoo/paper-muncher/pull/327) [#329](https://github.com/odoo/paper-muncher/pull/329) [#332](https://github.com/odoo/paper-muncher/pull/332) [#333](https://github.com/odoo/paper-muncher/pull/333) [#341](https://github.com/odoo/paper-muncher/pull/341)
 - Unified the representation of CSS dimensions [#337](https://github.com/odoo/paper-muncher/pull/337)
 - And many more random bug fixes all around the engine!

[GitHub Link](https://github.com/odoo/paper-muncher/releases/tag/v0.10.0)

---

# 🛠️ Paper Muncher v0.9.0

## Changes

 - Fixed many CSS parser crashes [#304](https://github.com/odoo/paper-muncher/pull/304) [#306](https://github.com/odoo/paper-muncher/pull/306) [#308](https://github.com/odoo/paper-muncher/pull/308)
 - Added support for `column-reverse` in flex layout [#310](https://github.com/odoo/paper-muncher/pull/310)
 - Added support for deprecated system colors [#322](https://github.com/odoo/paper-muncher/pull/322)
 - The `all` media type now matches every media [#315](https://github.com/odoo/paper-muncher/pull/315) [#316](https://github.com/odoo/paper-muncher/pull/316)
 - Improved logical properties, list markers and floated flex items [#311](https://github.com/odoo/paper-muncher/pull/311)
 - Improved table border handling [#307](https://github.com/odoo/paper-muncher/pull/307) [3ac944f](https://github.com/odoo/paper-muncher/commit/3ac944f5ee3dd5a599ef5ad5e435f8f7387a27fa)
 - Fixed implicit SVG `viewBox` anchoring [#313](https://github.com/odoo/paper-muncher/pull/313)
 - Fixed stacking contexts for boxes that imply one [a798666](https://github.com/odoo/paper-muncher/commit/a7986662a215355ba1a1f277aa74cea297c0e261)
 - And many more random bug fixes all around the engine!

[GitHub Link](https://github.com/odoo/paper-muncher/releases/tag/v0.9.0)

---

# 🏎️ Paper Muncher v0.8.0

## Changes

 - Completely reworked stacking context [#275](https://github.com/odoo/paper-muncher/pull/275)
 - Greatly improved sandbox hardening [2c2a840](https://github.com/odoo/paper-muncher/commit/2c2a8401e5e99422bba06888042c6b7bad4557e4) [2e9581e](https://github.com/odoo/paper-muncher/commit/2e9581e0d33e69b650f2729248bf24df1c2e936f)
 - Basic hit testing and working "inspect element" action in the inspector [#275](https://github.com/odoo/paper-muncher/pull/275)
 - New faster and simpler selector engine [#297](https://github.com/odoo/paper-muncher/pull/297) [#295](https://github.com/odoo/paper-muncher/pull/295) [#294](https://github.com/odoo/paper-muncher/pull/294) [#291](https://github.com/odoo/paper-muncher/pull/291)
 - Improved caching in the table layout algorithm [#293](https://github.com/odoo/paper-muncher/pull/293)
 - Sped up property cascading [#283](https://github.com/odoo/paper-muncher/pull/283) [#294](https://github.com/odoo/paper-muncher/pull/294) [#280](https://github.com/odoo/paper-muncher/pull/280) [#281](https://github.com/odoo/paper-muncher/pull/281)
 - Reduced memory usage for DOM attributes [#282](https://github.com/odoo/paper-muncher/pull/282)
 - Polyfilled wkhtmltopdf-style page numbering [#302](https://github.com/odoo/paper-muncher/pull/302)
 - And many more random bug fixes all around the engine!

[GitHub Link](https://github.com/odoo/paper-muncher/releases/tag/v0.8.0)

---

# 🎩 Paper-Muncher v0.7.1

## Changes

- Added support for `line-height`. [#269](https://github.com/odoo/paper-muncher/pull/269)
- Added support for the `vertical-align` and `baseline-shift`
  properties. [#267](https://github.com/odoo/paper-muncher/pull/267)
- Added height distribution for tables. [#266](https://github.com/odoo/paper-muncher/pull/266)
- Improved tables: row height distribution now agrees with other browsers, and table cells support
  `vertical-align`. [#268](https://github.com/odoo/paper-muncher/pull/268)
- Added support for `position: relative` in all formatting
  contexts. [#261](https://github.com/odoo/paper-muncher/pull/261)
- Improved absolute positioning in flex containers. [#264](https://github.com/odoo/paper-muncher/pull/264)
- Expanded the three argument `rotate()` of SVG into a
  `matrix()`. [#262](https://github.com/odoo/paper-muncher/pull/262)
- Made CSS token comparison case insensitive. [#264](https://github.com/odoo/paper-muncher/pull/264)
- Added Ubuntu Resolute as a packaging
  target. [`92b5567`](https://github.com/odoo/paper-muncher/commit/92b55677ef7a6090e4b2ec40f703977b89e337a2)

[GitHub Link](https://github.com/odoo/paper-muncher/releases/tag/v0.7.1)

---

# 🖌️ Paper-Muncher v0.6.0

## Changes

- Extended support for absolute and fixed positioning. [#256](https://github.com/odoo/paper-muncher/pull/256)
- Improved value resolution for `font-size`. [#257](https://github.com/odoo/paper-muncher/pull/257)
- Added support for `::marker` and `list-type` CSS properties. [#257](https://github.com/odoo/paper-muncher/pull/257)
- Added `--header`, `--footer`, `--header-size`, and `--footer-size` flags to the
  CLI. [#257](https://github.com/odoo/paper-muncher/pull/250)
- `--margins "<left> <top> <right> <bottom>"` is now supported in the
  CLI. [#257](https://github.com/odoo/paper-muncher/pull/250)
- Fixed a crash related to missing table cells. [#251](https://github.com/odoo/paper-muncher/pull/251)
- Improved the handling of guaranteed invalid CSS properties. [#251](https://github.com/odoo/paper-muncher/pull/248)
- Dumped the backtrace to stderr when panicking.
- Greatly improved support for markdown.

[GitHub Link](https://github.com/odoo/paper-muncher/releases/tag/v0.6.0)

---

# ☕ Paper-Muncher v0.5.0

## Changes

- Added smarter font fetching (PR [#244](https://github.com/odoo/paper-muncher/pull/244))
- Improved rich text support (PR [#247](https://github.com/odoo/paper-muncher/pull/247))
- Added support for `whitespace: no-wrap`  (PR [#238](https://github.com/odoo/paper-muncher/pull/238))
- Added support for `<center>` and `text-align: -vaev-block-center;` (
  PR [#233](https://github.com/odoo/paper-muncher/pull/233))
- Added support for the `align=` HTML attribute (PR [#233](https://github.com/odoo/paper-muncher/pull/233))
- Added support for `break: avoid-page;` (PR [#232](https://github.com/odoo/paper-muncher/pull/232))
- Fixed build on Linux Mint (PR [#235](https://github.com/odoo/paper-muncher/pull/235))
- Fixed build on MacOS
- Fixed `--sandbox` crashing

[GitHub Link](https://github.com/odoo/paper-muncher/releases/tag/v0.5.0)

---

# 🏎️ Paper-Muncher v0.4.1

## Changes

- Fixed package metadata for Debian based distributions.

[GitHub Link](https://github.com/odoo/paper-muncher/releases/tag/v0.4.1)

---

# 🏎️ Paper-Muncher v0.4.0

This is a hotfix release following all the feedback and issues reported after v0.3.0, thanks to everyone who tested and
reported bugs!

Going forward, we'll be aiming for a weekly release schedule until OXP.

## Changes

- Initial support for `margin: auto`
- Initial support for image sizing
- Image compression in the PDF backend
- Image transparency in the PDF backend
- Opacity support in the PDF backend
- New generic amd64 Linux build for distributions not yet covered by our packaging infrastructure

[GitHub Link](https://github.com/odoo/paper-muncher/releases/tag/v0.4.0)

---
