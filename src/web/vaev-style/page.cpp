module;

#include <karm-base/enum.h>
#include <karm-base/rc.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-io/emit.h>
#include <karm-logger/logger.h>

export module Vaev.Style:page;

import :decls;
import :origin;
import :props;

namespace Vaev::Style {

static constexpr bool DEBUG_PAGE = false;

// https://drafts.csswg.org/css-page-3/#margin-at-rules
#define FOREACH_PAGE_AREA(ITER)                      \
    ITER(FOOTNOTE, "footnote")                       \
    ITER(TOP, "-vaev-top")                           \
    ITER(TOP_LEFT_CORNER, "top-left-corner")         \
    ITER(TOP_LEFT, "top-left")                       \
    ITER(TOP_CENTER, "top-center")                   \
    ITER(TOP_RIGHT, "top-right")                     \
    ITER(TOP_RIGHT_CORNER, "top-right-corner")       \
    ITER(RIGHT, "-vaev-right")                       \
    ITER(RIGHT_TOP, "right-top")                     \
    ITER(RIGHT_MIDDLE, "right-middle")               \
    ITER(RIGHT_BOTTOM, "right-bottom")               \
    ITER(BOTTOM, "-vaev-bottom")                     \
    ITER(BOTTOM_RIGHT_CORNER, "bottom-right-corner") \
    ITER(BOTTOM_RIGHT, "bottom-right")               \
    ITER(BOTTOM_CENTER, "bottom-center")             \
    ITER(BOTTOM_LEFT, "bottom-left")                 \
    ITER(BOTTOM_LEFT_CORNER, "bottom-left-corner")   \
    ITER(LEFT, "-vaev-left")                         \
    ITER(LEFT_BOTTOM, "left-bottom")                 \
    ITER(LEFT_MIDDLE, "left-middle")                 \
    ITER(LEFT_TOP, "left-top")

export enum struct PageArea {
#define ITER(ID, ...) ID,
    FOREACH_PAGE_AREA(ITER)
#undef ITER
        _LEN,
};

export struct Page {
    String name = ""s;
    usize number;
    bool blank;
};

export struct PageComputedStyle {
    using Areas = Array<Rc<Computed>, toUnderlyingType(PageArea::_LEN)>;

    Rc<Computed> style;
    Areas _areas;

    PageComputedStyle(Computed const& initial)
        : style(makeRc<Computed>(initial)),
          _areas(Areas::fill([&](...) {
              return makeRc<Computed>(initial);
          })) {}

    Rc<Computed> area(PageArea margin) const {
        return _areas[toUnderlyingType(margin)];
    }
};

// https://drafts.csswg.org/css-page-3/#at-page-rule

#define FOREACH_PAGE_PSEUDO(ITER) \
    ITER(FIRST, "first")          \
    ITER(BLANK, "blank")          \
    ITER(LEFT, "left")            \
    ITER(RIGHT, "right")

export enum struct PagePseudo {
#define ITER(ID, ...) ID,
    FOREACH_PAGE_PSEUDO(ITER)
#undef ITER
};

// MARK: Page Selector ----------------------------------------------------------

export struct PageSelector {
    String name = ""s;
    Vec<PagePseudo> pseudos;

    static PageSelector parse(Cursor<Css::Sst>& c) {
        PageSelector res;

        if (c.peek() == Css::Token::IDENT) {
            res.name = c.next().token.data;
        }

        return res;
    }

    static Vec<PageSelector> parseList(Cursor<Css::Sst>& c) {
        Vec<PageSelector> res;

        eatWhitespace(c);
        while (not c.ended()) {
            res.pushBack(parse(c));
            eatWhitespace(c);
        }

        return res;
    }

    bool match(Page const& page) const {
        if (name and page.name != name)
            return false;

        for (auto const& pseudo : pseudos) {
            switch (pseudo) {
            case PagePseudo::FIRST:
                if (not page.number)
                    return false;
                break;
            case PagePseudo::BLANK:
                if (page.blank)
                    return false;
                break;
            case PagePseudo::LEFT:
                if (page.number % 2 == 0)
                    return false;
                break;
            case PagePseudo::RIGHT:
                if (page.number % 2 == 1)
                    return false;
                break;
            }
        }

        return true;
    }

    void repr(Io::Emit& e) const {
        e("({} pseudos: {})", name, pseudos);
    }
};

// MARK: Page Margin Rule ------------------------------------------------------

static Opt<PageArea> _parsePageArea(Css::Token tok) {
    Str name = next(tok.data);

#define ITER(ID, NAME) \
    if (name == NAME)  \
        return PageArea::ID;
    FOREACH_PAGE_AREA(ITER)
#undef ITER

    logWarn("unknown page area: {}", name);

    return NONE;
}

export struct PageAreaRule {
    PageArea area;
    Vec<StyleProp> props;

    static Opt<PageAreaRule> parse(Css::Sst const& sst) {
        PageAreaRule res;

        res.area = try$(_parsePageArea(sst.token));

        for (auto const& item : sst.content) {
            if (item == Css::Sst::DECL) {
                auto prop = parseDeclaration<StyleProp>(item);
                if (prop)
                    res.props.pushBack(prop.take());
            } else {
                logWarnIf(DEBUG_PAGE, "unexpected item in style rule: {}", item);
            }
        }

        return res;
    }

    void apply(Computed& c) const {
        for (auto const& prop : props) {
            prop.apply(c, c);
        }
    }

    void repr(Io::Emit& e) const {
        e("(page-margin-rule\nmargin: {}\nprops: {})", area, props);
    }
};

// MARK: Page Rule -------------------------------------------------------------

export struct PageRule {
    Vec<PageSelector> selectors;
    Vec<StyleProp> props;
    Vec<PageAreaRule> areas;

    static PageRule parse(Css::Sst const& sst) {
        if (sst != Css::Sst::RULE)
            panic("expected rule");

        if (sst.prefix != Css::Sst::LIST)
            panic("expected list");

        PageRule res;

        // Parse the selector
        auto& prefix = sst.prefix.unwrap();
        Cursor<Css::Sst> prefixContent = prefix->content;
        res.selectors = PageSelector::parseList(prefixContent);

        // Parse the properties.
        for (auto const& item : sst.content) {
            if (item == Css::Sst::DECL) {
                auto prop = parseDeclaration<StyleProp>(item);
                if (prop)
                    res.props.pushBack(prop.take());
            } else if (item == Css::Sst::RULE and
                       item.token == Css::Token::AT_KEYWORD) {
                auto rule = PageAreaRule::parse(item);
                if (rule)
                    res.areas.pushBack(*rule);
            } else {
                logWarnIf(DEBUG_PAGE, "unexpected item in style rule: {}", item);
            }
        }

        return res;
    }

    bool match(Page const& page) const {
        if (selectors.len() == 0)
            return true;

        for (auto& s : selectors) {
            if (s.match(page))
                return true;
        }

        return false;
    }

    void apply(PageComputedStyle& c) const {
        for (auto const& prop : props) {
            prop.apply(*c.style, *c.style);
        }

        for (auto const& area : areas) {
            auto computed = c.area(area.area);
            area.apply(*computed);
        }
    }

    void repr(Io::Emit& e) const {
        e("(page-rule\nselectors: {}\nprops: {}\nmargins: {})", selectors, props, areas);
    }
};

} // namespace Vaev::Style
