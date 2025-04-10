#include <karm-gc/heap.h>
#include <karm-test/macros.h>
#include <vaev-dom/document.h>
#include <vaev-style/media.h>
#include <karm-text/prose.h>
#include <vaev-base/display.h>
#include <vaev-dom/html/parser.h>

import Vaev.Driver;
import Vaev.Layout;

namespace Vaev::Layout::Tests {

static Style::Media const TEST_MEDIA = {
    .type = MediaType::SCREEN,
    .width = 1920_au,
    .height = 1080_au,
    .aspectRatio = 16.0 / 9.0,
    .orientation = Print::Orientation::LANDSCAPE,

    .resolution = Resolution::fromDpi(96),
    .scan = Scan::PROGRESSIVE,
    .grid = false,
    .update = Update::NONE,
    .overflowBlock = OverflowBlock::NONE,
    .overflowInline = OverflowInline::NONE,

    .color = 8,
    .colorIndex = 256,
    .monochrome = 0,
    .colorGamut = ColorGamut::SRGB,
    .pointer = Pointer::NONE,
    .hover = Hover::NONE,
    .anyPointer = Pointer::FINE,
    .anyHover = Hover::HOVER,

    .prefersReducedMotion = ReducedMotion::REDUCE,
    .prefersReducedTransparency = ReducedTransparency::NO_PREFERENCE,
    .prefersContrast = Contrast::LESS,
    .forcedColors = Colors::NONE,
    .prefersColorScheme = ColorScheme::LIGHT,
    .prefersReducedData = ReducedData::REDUCE,

    .deviceWidth = 1920_au,
    .deviceHeight = 1080_au,
    .deviceAspectRatio = 16.0 / 9.0,
};

struct FakeBox {
    bool stablishesInline;
    bool isBlockLevel;
    Vec<FakeBox> children{};

    bool matches(Box const& b) {
        if (not b.style->display.is(Display::Type::DEFAULT))
            return false;

        bool boxStablishesInline = b.content.is<Layout::InlineBox>();
        bool boxIsBlockLevel = (b.style->display.outside() == Display::Outside::BLOCK);

        // logDebug("box: {} {}, expected: {} {}", boxStablishesInline, boxIsBlockLevel, stablishesInline, isBlockLevel);

        if (boxIsBlockLevel != isBlockLevel)
            return false;

        if (boxStablishesInline != stablishesInline)
            return false;

        if (boxStablishesInline) {
            if (children.len() != 1)
                return false;
            auto rootInlineBox = FakeInlineBox::fromInlineBoxSpanTree(b.content.unwrap<Layout::InlineBox>().prose->_spans);
            return children[0].matches(rootInlineBox);
        } else {
            // logDebug("box children: {} expected children: {}", b.children().len(), children.len());
            if (children.len() != b.children().len())
                return false;

            for (usize i = 0; i < children.len(); ++i) {
                if (not children[i].matches(b.children()[i]))
                    return false;
            }
            return true;
        }
    }

    struct FakeInlineBox {
        Vec<FakeInlineBox> children{};

        FakeInlineBox& add(FakeInlineBox&& box) {
            children.pushBack(std::move(box));
            return last(children);
        }

        static FakeInlineBox fromInlineBoxSpanTree(Vec<::Rc<Text::Prose::Span>> const& spans) {
            FakeInlineBox inlineBoxTree;

            Vec<MutCursor<FakeInlineBox>> stackInlineBoxes = {&inlineBoxTree};
            Vec<Opt<Rc<Text::Prose::Span>>> stackSpans = {NONE};
            for (auto& span : spans) {
                while (true) {
                    if (span->parent == NONE) {
                        if (last(stackSpans) == NONE)
                            break;

                        (void)stackSpans.popBack();
                        stackInlineBoxes.popBack();
                    } else {
                        if (span->parent->_cell == last(stackSpans)->_cell)
                            break;

                        (void)stackSpans.popBack();
                        stackInlineBoxes.popBack();
                    }
                }

                stackSpans.pushBack(span);
                stackInlineBoxes.pushBack(&last(stackInlineBoxes)->add(FakeInlineBox{}));
            }

            return inlineBoxTree;
        }
    };

    bool matches(FakeInlineBox const& b) {
        // FIXME: fix this when we have support to inline-block
        if (isBlockLevel or not stablishesInline)
            return false;

        if (children.len() != b.children.len())
            return false;

        for (usize i = 0; i < children.len(); ++i) {
            if (not children[i].matches(b.children[i]))
                return false;
        }
        return true;
    }


};

test$("empty-body") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html></body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .stablishesInline = false,
            .isBlockLevel = true,
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("no span") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><body>hello, world</body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .stablishesInline = true,
            .isBlockLevel = true,
            .children{
                FakeBox{
                    // root inline with "hello, world"
                    .stablishesInline = true,
                    .isBlockLevel = false,
                }
            }
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("no span") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><body>hello,<br>brrrrr world</body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .stablishesInline = false,
            .isBlockLevel = true,
            .children{
                FakeBox{
                    // anon box for hello,
                    .stablishesInline = true,
                    .isBlockLevel = true,
                    .children{
                        FakeBox{
                            // root inline with "hello,"
                            .stablishesInline = true,
                            .isBlockLevel = false,
                        },
                    }
                },
                FakeBox{
                    // anon box brrrrr world
                    .stablishesInline = true,
                    .isBlockLevel = true,
                    .children{
                        FakeBox{
                            // root inline with "brrrrr world"
                            .stablishesInline = true,
                            .isBlockLevel = false,
                        },
                    }
                },
            }
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("no span, breaking block") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><body>hello, <div>cruel</div> world</body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .stablishesInline = false,
            .isBlockLevel = true,
            .children = {
                FakeBox{
                    // anon block
                    .stablishesInline = true,
                    .isBlockLevel = true,
                    .children{
                        FakeBox{
                            // root inline with "hello, "
                            .stablishesInline = true,
                            .isBlockLevel = false,
                        }
                    }
                },
                FakeBox{
                    // div block
                    .stablishesInline = true,
                    .isBlockLevel = true,
                    .children{
                        FakeBox{
                            // root inline with "cruel"
                            .stablishesInline = true,
                            .isBlockLevel = false,
                        }
                    }
                },
                FakeBox{
                    // anon block
                    .stablishesInline = true,
                    .isBlockLevel = true,
                    .children{
                        FakeBox{
                            // root inline with "world"
                            .stablishesInline = true,
                            .isBlockLevel = false,
                        }
                    }
                },
            }
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("span and breaking block 1") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><body>"
        "<span>hello"
        "<span>cruel"
        "<span>world"
        "<div></div>"
        "</span></span></span>"
        "melancholy"
        "</body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .stablishesInline = false,
            .isBlockLevel = true,
            .children = {
                FakeBox{
                    // anon div for inlines
                    .stablishesInline = true,
                    .isBlockLevel = true,
                    .children{
                        FakeBox{
                            // root inline box
                            .stablishesInline = true,
                            .isBlockLevel = false,
                            .children{
                                FakeBox{
                                    // span with hello
                                    .stablishesInline = true,
                                    .isBlockLevel = false,
                                    .children = {
                                        FakeBox{
                                            // span with cruel
                                            .stablishesInline = true,
                                            .isBlockLevel = false,
                                            .children = {
                                                FakeBox{
                                                    // span with world
                                                    .stablishesInline = true,
                                                    .isBlockLevel = false,
                                                },
                                            },
                                        },
                                    },
                                },
                            },
                        },
                    },
                },
                FakeBox{
                    // div block
                    .stablishesInline = false,
                    .isBlockLevel = true,
                },
                FakeBox{
                    .stablishesInline = true,
                    .isBlockLevel = true,
                    .children = {
                        FakeBox{
                            // root inline box
                            .stablishesInline = true,
                            .isBlockLevel = false,
                            .children = {
                                FakeBox{
                                    // span that had hello
                                    .stablishesInline = true,
                                    .isBlockLevel = false,
                                    .children = {
                                        FakeBox{
                                            // span that had cruel
                                            .stablishesInline = true,
                                            .isBlockLevel = false,
                                            .children = {FakeBox{
                                                // span that had world
                                                .stablishesInline = true,
                                                .isBlockLevel = false,
                                            }},
                                        },
                                    },
                                },
                                // root inline with have "melancholy"
                            },
                        },
                    },
                },
            }
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("span and breaking block 2") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><body>"
        "<span>hello"
        "<span>cruel"
        "<span>world"
        "<div></div>"
        "melancholy"
        "</span></span><div>kidding</div></span>"
        "good vibes"
        "</body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .stablishesInline = false,
            .isBlockLevel = true,
            .children = {
                FakeBox{
                    // anon div for inlines
                    .stablishesInline = true,
                    .isBlockLevel = true,
                    .children{
                        FakeBox{
                            // root inline box
                            .stablishesInline = true,
                            .isBlockLevel = false,
                            .children{
                                FakeBox{
                                    // hello
                                    .stablishesInline = true,
                                    .isBlockLevel = false,
                                    .children = {
                                        FakeBox{
                                            // cruel
                                            .stablishesInline = true,
                                            .isBlockLevel = false,
                                            .children = {
                                                FakeBox{
                                                    // world
                                                    .stablishesInline = true,
                                                    .isBlockLevel = false,
                                                },
                                            },
                                        },
                                    },
                                },
                            },
                        },
                    },
                },
                FakeBox{
                    // misplaced div 1
                    .stablishesInline = false,
                    .isBlockLevel = true,
                },
                FakeBox{
                    // anon div for inline
                    .stablishesInline = true,
                    .isBlockLevel = true,
                    .children = {
                        FakeBox{
                            // root inline box
                            .stablishesInline = true,
                            .isBlockLevel = false,
                            .children = {
                                FakeBox{
                                    // span with hello
                                    .stablishesInline = true,
                                    .isBlockLevel = false,
                                    .children = {
                                        FakeBox{
                                            // span with cruel
                                            .stablishesInline = true,
                                            .isBlockLevel = false,
                                            .children = {
                                                FakeBox{
                                                    // span with melancholy
                                                    .stablishesInline = true,
                                                    .isBlockLevel = false,
                                                },
                                            },
                                        },
                                    },
                                },
                            },
                        },
                    },
                },
                FakeBox{
                    // misplaced div 2
                    .stablishesInline = true,
                    .isBlockLevel = true,
                    .children = {
                        FakeBox{
                            // root inline box with "kidding"
                            .stablishesInline = true,
                            .isBlockLevel = false,
                        },
                    },
                },
                FakeBox{
                    // anon div for inline
                    .stablishesInline = true,
                    .isBlockLevel = true,
                    .children = {
                        FakeBox{
                            // root inline box
                            .stablishesInline = true,
                            .isBlockLevel = false,
                            .children = {
                                FakeBox{
                                    // span that had "hello"
                                    .stablishesInline = true,
                                    .isBlockLevel = false,
                                },
                                // root inline with have good vibes in the end
                            },
                        },
                    },
                },
            }
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

} // namespace Vaev::Layout::Tests
