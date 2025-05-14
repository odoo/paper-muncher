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
    usize page;
    RunningPosition running;
    Gc::Ref<Dom::Element> element;

    RunningPositionInfo(usize page, RunningPosition running, Gc::Ref<Dom::Element> element)
        : page(page), running(running), element(element) {
    }

    void repr(Io::Emit& e) const {
        e("runningPosInfos \nrunning:{} \npage:{} \nelement:{}", running, page, element);
    }
};

struct RunningPositionMap {
    Map<CustomIdent, Vec<RunningPositionInfo>> content;

    void add(usize pageNumber, Layout::Box& box) {
        auto& style = box.style;

        if (auto position = style->position.is<RunningPosition>()) {
            auto const origin = box.origin;
            if (box.origin == nullptr)
                return;
            RunningPositionInfo info = {pageNumber, *position, origin.upgrade()};
            content.getOrDefault(position->customIdent)
                .pushBack(std::move(info));
        }
    }

    // https://www.w3.org/TR/css-gcpm-3/#using-named-strings
    Res<RunningPositionInfo> match(ElementContent elt, usize currentPage = 0) {
        auto id = elt.customIdent;
        if (not content.has(id)) {
            return Error::notFound("element not found");
        }

        auto const& list = content.get(id);

        switch (elt.target) {
        case ElementContent::Target::UNDEFINED:
            return Ok(list[0]);

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

    Slice<RunningPositionInfo> _searchPage(Slice<RunningPositionInfo> list, usize page) {
        page++; // pages are 1-indexed
        // binary search of all running positions that match the page

        auto res = search(list, [&](RunningPositionInfo const& info) {
            if (info.page == page) {
                return std::strong_ordering::equal;
            }
            return info.page <=> page;
        });

        if (not res) {
            return sub(list, 0, 1);
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
