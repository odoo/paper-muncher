export module Vaev.Engine:style.values.primitives;

import Karm.Core;

import :css.values.primitives;

using namespace Karm;

namespace Vaev::Style {

// MARK: Integer ---------------------------------------------------------------
// https://drafts.csswg.org/css-values/#integers

export using Integer = isize;

// MARK: Custom Ident ----------------------------------------------------------
// https://www.w3.org/TR/css-values-4/#identifier-value

export using CustomIdent = Css::CustomIdent;

} // namespace Vaev::Style
