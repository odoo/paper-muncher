module;

#include <karm/macros>

export module Vaev.Engine:layout.runningPosition;

import Karm.Core;
import Karm.Math;

import :values.insets;
import :values.content;
import :layout.box;

using namespace Karm;

namespace Vaev::Layout {

// MARK: RunningPos ------------------------------------------------------------

export struct RunningPositionInfo {
    usize pageNumber;
    RunningPosition running;
    Dom::OriginatingElement element;

    RunningPositionInfo(usize page, RunningPosition running, Dom::OriginatingElement element)
        : pageNumber(page), running(running), element(element) {
    }

    void repr(Io::Emit& e) const {
        e("(runningPosInfos running:{} page:{} element:{})", running, pageNumber, element);
    }
};

// Mapping the different Running positions to their respective names and their page.
struct RunningPositionMap {
    Map<CustomIdent, Vec<RunningPositionInfo>> content;

    void add(usize pageNumber, Layout::Box& box) {
        auto& style = box.style;

        if (auto position = style->position.is<RunningPosition>()) {
            auto const origin = box.origin;
            if (not box.origin)
                return;
            RunningPositionInfo info = {pageNumber, *position, origin.unwrap()};
            content.lookupOrPutDefault(position->customIdent)
                .pushBack(std::move(info));
        }
    }

    // https://www.w3.org/TR/css-gcpm-3/#using-named-strings
    Res<RunningPositionInfo> match(ElementFunc elt, usize pageNumber) {
        auto id = elt.customIdent;
        auto const& list = try$(content.lookup(id).okOr(Error::notFound("element not found")));

        switch (elt.target) {
        case ElementFunc::Target::UNDEFINED:
            return Ok(list[0]);

        case ElementFunc::Target::START:
            for (usize i = 0; i < list.len(); i++) {
                auto elt = list[i];
                if (elt.pageNumber == pageNumber and i > 0) {
                    return Ok(list[i - 1]);
                }
            }
            return Ok(list[0]);

        case ElementFunc::Target::FIRST:
        case ElementFunc::Target::FIRST_EXCEPT: {
            auto elements = _searchPage(list, pageNumber);
            return Ok(elements[0]);
        }

        case ElementFunc::Target::LAST: {
            auto elements = _searchPage(list, pageNumber);
            return Ok(elements[elements.len() - 1]);
        }
        }
    }

    Slice<RunningPositionInfo> _searchPage(Slice<RunningPositionInfo> list, usize page) {
        // binary search of all running positions that match the page
        auto res = search(list, [&](RunningPositionInfo const& info) {
            if (info.pageNumber == page) {
                return std::strong_ordering::equal;
            }
            return info.pageNumber <=> page;
        });

        if (not res)
            return sub(list, 0, 1);

        // a random element of the page
        auto index = res.take();

        // search left side for first element of the page
        usize l = index;
        while (l > 0 and list[l - 1].pageNumber == page)
            l--;

        // search right side for last element of the page
        usize r = index;
        while (r < list.len() - 1 and list[r + 1].pageNumber == page)
            r++;

        return sub(list, l, r + 1);
    }

    void repr(Io::Emit& e) const {
        e("{}", content);
    }
};

} // namespace Vaev::Layout
