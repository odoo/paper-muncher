export module Vaev.Engine:layout.runningPos;

import Karm.Core;
import Karm.Math;

import :values.insets;
import :values.content;
import :layout.box;

using namespace Karm;

namespace Vaev::Layout {

// MARK: RunningPos ------------------------------------------------------------

export struct RunningPositionInfo {
    usize page;
    RunningPosition running;
    Box structure;

    RunningPositionInfo(usize page, RunningPosition running, Box structure) : page(page), running(running), structure(structure) {
    }

    void repr(Io::Emit& e) const {
        e("runningPosInfos '\nrunning:{} \npage:{} \nstructure:{}'", running, page, structure);
    }
};

struct RunningPositionMap {
    Map<CustomIdent, Vec<RunningPositionInfo>> content;

    void add(usize pageNumber, Layout::Box& box) {

        auto& style = box.style;
        if (auto pos = style->position.is<RunningPosition>()) {
            auto position = pos.peek();

            Box copy = box;
            copy.style = makeRc<Style::SpecifiedValues>(*style);
            copy.style->position = Keywords::STATIC;
            RunningPositionInfo info = {pageNumber, position, copy};
            if (content.has(position.customIdent)) {
                content.get(position.customIdent).pushBack(info);
            } else {
                content.put(position.customIdent, {info});
            }
        }
    }

    // https://www.w3.org/TR/css-gcpm-3/#using-named-strings
    Res<RunningPositionInfo> match(ElementContent elt, usize currentPage = 0) {
        auto id = elt.customIdent;
        if (not content.has(id)) {
            return Error::notFound("element not found");
        }

        auto list = content.get(id);

        switch (elt.target) {
        case ElementContent::Target::UNDEFINED:
            return Ok(list[0]);
            break;
        case ElementContent::Target::START:
            for (usize i = 0; i < list.len(); i++) {
                auto elt = list[i];
                if (elt.page == currentPage and i > 0) {
                    return Ok(list[i - 1]);
                }
            }
            return Ok(list[0]);
        case ElementContent::Target::FIRST:
        case ElementContent::Target::FIRST_EXCEPT: {
            auto elements = _searchPage(list, currentPage);
            return Ok(elements[0]);
        }
        case ElementContent::Target::LAST: {
            auto elements = _searchPage(list, currentPage);
            return Ok(elements[elements.len() - 1]);
        }
        }
    }

    Vec<RunningPositionInfo> _searchPage(Vec<RunningPositionInfo> list, usize page) {
        page++; // pages are 1-indexed
        // binary search of all running positions that match the page

        auto res = search(list, [&](RunningPositionInfo const& info) {
            if (info.page == page) {
                return std::strong_ordering::equal;
            }
            return info.page <=> page;
        });

        if (not res) {
            return {list[0]};
        }

        // a random element of the page
        auto index = res.take();

        // search left side for first element of the page
        usize l = index;
        while (l > 0 and list[l - 1].page == page) {
            l--;
        }

        // search right side for last element of the page
        usize r = index;
        while (r < list.len() - 1 and list[r + 1].page == page) {
            r++;
        }

        return sub(list, l, r + 1);
    }

    void repr(Io::Emit& e) const {
        e("{}", content);
    }
};

} // namespace Vaev::Layout
