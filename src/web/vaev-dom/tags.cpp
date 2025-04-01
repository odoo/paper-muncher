#include "tags.h"

namespace Vive::Html {

Str _tagName(TagId id) {
    switch (id) {
#define TAG(IDENT, NAME) \
    case TagId::IDENT:   \
        return #NAME;
#include "defs/ns-html-tag-names.inc"
#undef TAG
    default:
        return "unknown";
    }
}

Str _attrName(AttrId id) {
    switch (id) {
#define ATTR(IDENT, NAME) \
    case AttrId::IDENT:   \
        return #NAME;
#include "defs/ns-html-attr-names.inc"
#undef ATTR
    default:
        return "unknown";
    }
}

Opt<TagId> _tagId(Str name) {
#define TAG(IDENT, NAME) \
    if (name == #NAME)   \
        return TagId::IDENT;
#include "defs/ns-html-tag-names.inc"
#undef TAG

    return NONE;
}

Opt<AttrId> _attrId(Str name) {
#define ATTR(IDENT, NAME) \
    if (name == #NAME)    \
        return AttrId::IDENT;

#include "defs/ns-html-attr-names.inc"
#undef ATTR

    return NONE;
}

} // namespace Vive::Html

namespace Vive::MathMl {

Str _tagName(TagId id) {
    switch (id) {
#define TAG(IDENT, NAME) \
    case TagId::IDENT:   \
        return #NAME;
#include "defs/ns-mathml-tag-names.inc"
#undef TAG
    default:
        return "unknown";
    }
}

Str _attrName(AttrId id) {
    switch (id) {
#define ATTR(IDENT, NAME) \
    case AttrId::IDENT:   \
        return #NAME;
#include "defs/ns-mathml-attr-names.inc"
#undef ATTR
    default:
        return "unknown";
    }
}

Opt<TagId> _tagId(Str name) {
#define TAG(IDENT, NAME) \
    if (name == #NAME)   \
        return TagId::IDENT;
#include "defs/ns-mathml-tag-names.inc"
#undef TAG

    return NONE;
}

Opt<AttrId> _attrId(Str name) {
#define ATTR(IDENT, NAME) \
    if (name == #NAME)    \
        return AttrId::IDENT;

#include "defs/ns-mathml-attr-names.inc"
#undef ATTR

    return NONE;
}

} // namespace Vive::MathMl

namespace Vive::Svg {

Str _tagName(TagId id) {
    switch (id) {
#define TAG(IDENT, NAME) \
    case TagId::IDENT:   \
        return #NAME;
#include "defs/ns-svg-tag-names.inc"
#undef TAG
    default:
        return "unknown";
    }
}

Str _attrName(AttrId id) {
    switch (id) {
#define ATTR(IDENT, NAME) \
    case AttrId::IDENT:   \
        return #NAME;
#include "defs/ns-svg-attr-names.inc"
#undef ATTR
    default:
        return "unknown";
    }
}

Opt<TagId> _tagId(Str name) {
#define TAG(IDENT, NAME) \
    if (name == #NAME)   \
        return TagId::IDENT;
#include "defs/ns-svg-tag-names.inc"
#undef TAG

    return NONE;
}

Opt<AttrId> _attrId(Str name) {
#define ATTR(IDENT, NAME) \
    if (name == #NAME)    \
        return AttrId::IDENT;

#include "defs/ns-svg-attr-names.inc"
#undef ATTR

    return NONE;
}

} // namespace Vive::Svg
