module;

#include <karm-core/macros.h>

export module Vaev.Engine:style.props;

import Karm.Core;
import Karm.Ref;
import Karm.Gfx;
import Karm.Math;

import :values;
import :css;
import :style.specified;

using namespace Karm;

// https://www.w3.org/TR/CSS22/propidx.html

namespace Vaev::Style {

export struct Property {
    struct Declaration {
        virtual ~Declaration() = default;

        virtual Symbol name() const = 0;

        virtual Rc<Property> initial() const = 0;

        virtual Rc<Property> load(SpecifiedValues const& c) const = 0;

        virtual Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const = 0;
    };

    bool important = false;

    virtual ~Property() = default;

    virtual void apply(SpecifiedValues& c) const = 0;

    virtual void apply(SpecifiedValues const& parent, SpecifiedValues& child) const {
        apply(child);
    }

    virtual void repr(Io::Emit& e) const = 0;
};

export struct PropertyRegistry {
    Map<Symbol, Rc<Property::Declaration>> _decls;
};

// MARK: Props -----------------------------------------------------------------

// NOTE: This list should be kept alphabetically sorted.

// MARK: Align -----------------------------------------------------------------
// https://drafts.csswg.org/css-align-3

// https://drafts.csswg.org/css-align-3/#propdef-align-content
export struct AlignContentProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "align-content"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<AlignContentProperty>(Align::Keywords::STRETCH);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<AlignContentProperty>(c.aligns.alignContent);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignContentProperty>(try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    AlignContentProperty(Align value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.aligns.alignContent = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-content
export struct JustifyContentProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "justify-content"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<JustifyContentProperty>(Align::Keywords::FLEX_START);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<JustifyContentProperty>(c.aligns.justifyContent);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<JustifyContentProperty>(try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    JustifyContentProperty(Align value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.aligns.justifyContent = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-self
export struct JustifySelfProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "justify-self"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<JustifySelfProperty>(Align{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<JustifySelfProperty>(c.aligns.justifySelf);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<JustifySelfProperty>(try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    JustifySelfProperty(Align value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.aligns.justifySelf = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-align-self
export struct AlignSelfProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "align-self"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<AlignSelfProperty>(Align::Keywords::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<AlignSelfProperty>(c.aligns.alignSelf);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignSelfProperty>(try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    AlignSelfProperty(Align value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.aligns.alignSelf = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-items
export struct JustifyItemsProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "justify-items"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<JustifyItemsProperty>(Align{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<JustifyItemsProperty>(c.aligns.justifyItems);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<JustifyItemsProperty>(try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    JustifyItemsProperty(Align value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.aligns.justifyItems = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-align-items
export struct AlignItemsProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "align-items"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<AlignItemsProperty>(Align::Keywords::STRETCH);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<AlignItemsProperty>(c.aligns.alignItems);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignItemsProperty>(try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    AlignItemsProperty(Align value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.aligns.alignItems = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
export struct RowGapProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "row-gap"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<RowGapProperty>(Keywords::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<RowGapProperty>(c.gaps->y);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<RowGapProperty>(try$(parseValue<Gap>(c))));
        }
    };

    Gap _value;

    RowGapProperty(Gap value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.gaps.cow().y = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
export struct ColumnGapProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "column-gap"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<ColumnGapProperty>(Keywords::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ColumnGapProperty>(c.gaps->x);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ColumnGapProperty>(try$(parseValue<Gap>(c))));
        }
    };

    Gap _value;

    ColumnGapProperty(Gap value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.gaps.cow().x = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Baselines ------------------------------------------------------

// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
export struct DominantBaselineProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "dominant-baseline"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<DominantBaselineProperty>(Keywords::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<DominantBaselineProperty>(c.baseline->dominant);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<DominantBaselineProperty>(try$(parseValue<DominantBaseline>(c))));
        }
    };

    DominantBaseline _value;

    DominantBaselineProperty(DominantBaseline value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.baseline.cow().dominant = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-inline-3/#baseline-source
export struct BaselineSourceProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "baseline-source"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BaselineSourceProperty>(Keywords::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BaselineSourceProperty>(c.baseline->source);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BaselineSourceProperty>(try$(parseValue<BaselineSource>(c))));
        }
    };

    BaselineSource _value;

    BaselineSourceProperty(BaselineSource value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.baseline.cow().source = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-inline-3/#alignment-baseline-property
export struct AlignmentBaselineProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "alignment-baseline"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<AlignmentBaselineProperty>(Keywords::BASELINE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<AlignmentBaselineProperty>(c.baseline->alignment);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignmentBaselineProperty>(try$(parseValue<AlignmentBaseline>(c))));
        }
    };

    AlignmentBaseline _value;

    AlignmentBaselineProperty(AlignmentBaseline value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.baseline.cow().alignment = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Background Color ------------------------------------------------------

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-color
export struct BackgroundColorProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "background-color"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BackgroundColorProperty>(TRANSPARENT);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BackgroundColorProperty>(c.backgrounds->color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BackgroundColorProperty>(try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BackgroundColorProperty(Color value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.backgrounds.cow().color = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Background Image ------------------------------------------------------

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-attachment
export struct BackgroundAttachmentProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "background-attachment"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BackgroundAttachmentProperty>(Vec<BackgroundAttachment>{BackgroundAttachment::SCROLL});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            Vec<BackgroundAttachment> layers;
            for (auto const& l : c.backgrounds->layers)
                layers.pushBack(l.attachment);
            return makeRc<BackgroundAttachmentProperty>(std::move(layers));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BackgroundAttachmentProperty>(try$(parseValue<Vec<BackgroundAttachment>>(c))));
        }
    };

    Vec<BackgroundAttachment> _value;

    BackgroundAttachmentProperty(Vec<BackgroundAttachment> value) : _value(std::move(value)) {}

    void apply(SpecifiedValues& c) const override {
        auto& layers = c.backgrounds.cow().layers;
        layers.resize(max(layers.len(), _value.len()));
        for (usize i = 0; i < _value.len(); ++i)
            layers[i].attachment = _value[i];
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-image
export struct BackgroundImageProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "background-image"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BackgroundImageProperty>(Vec<Image>{});
        }

        Rc<Property> load(SpecifiedValues const&) const override {
            return makeRc<BackgroundImageProperty>(Vec<Image>{});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>&) const override {
            // TODO
            return Ok(makeRc<BackgroundImageProperty>(Vec<Image>{}));
        }
    };

    Vec<Image> _value;

    BackgroundImageProperty(Vec<Image> value) : _value(std::move(value)) {}

    void apply(SpecifiedValues&) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-position
export struct BackgroundPositionProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "background-position"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BackgroundPositionProperty>(Vec<BackgroundPosition>{});
        }

        Rc<Property> load(SpecifiedValues const&) const override {
            return makeRc<BackgroundPositionProperty>(Vec<BackgroundPosition>{});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>&) const override {
            // TODO
            return Ok(makeRc<BackgroundPositionProperty>(Vec<BackgroundPosition>{}));
        }
    };

    Vec<BackgroundPosition> _value;

    BackgroundPositionProperty(Vec<BackgroundPosition> value) : _value(std::move(value)) {}

    void apply(SpecifiedValues&) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-repeat
export struct BackgroundRepeatProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "background-repeat"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BackgroundRepeatProperty>(Vec<BackgroundRepeat>{BackgroundRepeat::REPEAT});
        }

        Rc<Property> load(SpecifiedValues const&) const override {
            return makeRc<BackgroundRepeatProperty>(Vec<BackgroundRepeat>{});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>&) const override {
            // TODO
            return Ok(makeRc<BackgroundRepeatProperty>(Vec<BackgroundRepeat>{}));
        }
    };

    Vec<BackgroundRepeat> _value;

    BackgroundRepeatProperty(Vec<BackgroundRepeat> value) : _value(std::move(value)) {}

    void apply(SpecifiedValues&) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#x10
export struct BackgroundProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "background"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BackgroundProperty>(BackgroundProps{TRANSPARENT});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BackgroundProperty>(*c.backgrounds);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            BackgroundProps value;
            value.color = try$(parseValue<Color>(c));
            return Ok(makeRc<BackgroundProperty>(std::move(value)));
        }
    };

    BackgroundProps _value;

    BackgroundProperty(BackgroundProps value) : _value(std::move(value)) {}

    void apply(SpecifiedValues& c) const override {
        c.backgrounds.cow() = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-color
export struct ColorProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "color"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<ColorProperty>(BLACK);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ColorProperty>(c.color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ColorProperty>(try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    ColorProperty(Color value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.color = resolve(_value, Gfx::BLACK);
    }

    void apply(SpecifiedValues const& parent, SpecifiedValues& c) const override {
        c.color = resolve(_value, parent.color);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-display
export struct DisplayProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "display"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<DisplayProperty>(Display{Display::FLOW, Display::INLINE});
        }

        Rc<Property> load(SpecifiedValues const& s) const override {
            return makeRc<DisplayProperty>(s.display);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<DisplayProperty>(try$(parseValue<Display>(c))));
        }
    };

    Display _value;

    DisplayProperty(Display value) : _value(value) {}

    void apply(SpecifiedValues& s) const override {
        s.display = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS21/tables.html#propdef-table-layout
export struct TableLayoutProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "table-layout"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<TableLayoutProperty>(TableLayout::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& s) const override {
            return makeRc<TableLayoutProperty>(s.table->tableLayout);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TableLayoutProperty>(try$(parseValue<TableLayout>(c))));
        }
    };

    TableLayout _value;

    TableLayoutProperty(TableLayout value) : _value(value) {}

    void apply(SpecifiedValues& s) const override {
        s.table.cow().tableLayout = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS21/tables.html#caption-position
export struct CaptionSideProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "caption-side"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<CaptionSideProperty>(CaptionSide::TOP);
        }

        Rc<Property> load(SpecifiedValues const& s) const override {
            return makeRc<CaptionSideProperty>(s.table->captionSide);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<CaptionSideProperty>(try$(parseValue<CaptionSide>(c))));
        }
    };

    CaptionSide _value;

    CaptionSideProperty(CaptionSide value) : _value(value) {}

    void apply(SpecifiedValues& s) const override {
        s.table.cow().captionSide = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Borders ---------------------------------------------------------------

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
export struct BorderTopColorProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-top-color"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderTopColorProperty>(BLACK);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderTopColorProperty>(c.borders->top.color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopColorProperty>(try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BorderTopColorProperty(Color value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().top.color = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
export struct BorderRightColorProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-right-color"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRightColorProperty>(BLACK);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRightColorProperty>(c.borders->end.color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightColorProperty>(try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BorderRightColorProperty(Color value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().end.color = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
export struct BorderBottomColorProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-bottom-color"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderBottomColorProperty>(BLACK);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderBottomColorProperty>(c.borders->bottom.color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomColorProperty>(try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BorderBottomColorProperty(Color value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().bottom.color = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
export struct BorderLeftColorProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-left-color"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderLeftColorProperty>(BLACK);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderLeftColorProperty>(c.borders->start.color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftColorProperty>(try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BorderLeftColorProperty(Color value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().start.color = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct BorderColorProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-color"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderColorProperty>(Math::Insets<Color>{BLACK});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderColorProperty>(Math::Insets<Color>{
                c.borders->start.color,
                c.borders->end.color,
                c.borders->top.color,
                c.borders->bottom.color,
            });
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderColorProperty>(try$(parseValue<Math::Insets<Color>>(c))));
        }
    };

    Math::Insets<Color> _value;

    BorderColorProperty(Math::Insets<Color> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        auto& borders = c.borders.cow();
        borders.start.color = _value.start;
        borders.end.color = _value.end;
        borders.top.color = _value.top;
        borders.bottom.color = _value.bottom;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties

export struct BorderStyleProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-style"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderStyleProperty>(Math::Insets<Gfx::BorderStyle>{Gfx::BorderStyle::NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderStyleProperty>(Math::Insets<Gfx::BorderStyle>{
                c.borders->start.style,
                c.borders->end.style,
                c.borders->top.style,
                c.borders->bottom.style,
            });
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderStyleProperty>(try$(parseValue<Math::Insets<Gfx::BorderStyle>>(c))));
        }
    };

    Math::Insets<Gfx::BorderStyle> _value;

    BorderStyleProperty(Math::Insets<Gfx::BorderStyle> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().start.style = _value.start;
        c.borders.cow().end.style = _value.end;
        c.borders.cow().top.style = _value.top;
        c.borders.cow().bottom.style = _value.bottom;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderLeftStyleProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-left-style"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderLeftStyleProperty>(Gfx::BorderStyle::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderLeftStyleProperty>(c.borders->start.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftStyleProperty>(try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderLeftStyleProperty(Gfx::BorderStyle value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().start.style = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderTopStyleProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-top-style"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderTopStyleProperty>(Gfx::BorderStyle::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderTopStyleProperty>(c.borders->top.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopStyleProperty>(try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderTopStyleProperty(Gfx::BorderStyle value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().top.style = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderRightStyleProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-right-style"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRightStyleProperty>(Gfx::BorderStyle::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRightStyleProperty>(c.borders->end.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightStyleProperty>(try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderRightStyleProperty(Gfx::BorderStyle value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().end.style = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderBottomStyleProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-bottom-style"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderBottomStyleProperty>(Gfx::BorderStyle::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderBottomStyleProperty>(c.borders->bottom.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomStyleProperty>(try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderBottomStyleProperty(Gfx::BorderStyle value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().bottom.style = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderTopWidthProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-top-width"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderTopWidthProperty>(Keywords::MEDIUM);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderTopWidthProperty>(c.borders->top.width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopWidthProperty>(try$(parseValue<LineWidth>(c))));
        }
    };

    LineWidth _value;

    BorderTopWidthProperty(LineWidth value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().top.width = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderRightWidthProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-right-width"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRightWidthProperty>(Keywords::MEDIUM);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRightWidthProperty>(c.borders->end.width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightWidthProperty>(try$(parseValue<LineWidth>(c))));
        }
    };

    LineWidth _value;

    BorderRightWidthProperty(LineWidth value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().end.width = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderBottomWidthProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-bottom-width"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderBottomWidthProperty>(Keywords::MEDIUM);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderBottomWidthProperty>(c.borders->bottom.width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomWidthProperty>(try$(parseValue<CalcValue<Length>>(c))));
        }
    };

    LineWidth _value;

    BorderBottomWidthProperty(LineWidth value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().bottom.width = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderLeftWidthProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-left-width"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderLeftWidthProperty>(Keywords::MEDIUM);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderLeftWidthProperty>(c.borders->start.width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftWidthProperty>(try$(parseValue<LineWidth>(c))));
        }
    };

    LineWidth _value;

    BorderLeftWidthProperty(LineWidth value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().start.width = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusTopRightProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-top-right-radius"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRadiusTopRightProperty>(makeArray<CalcValue<PercentOr<Length>>, 2>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRadiusTopRightProperty>(Array<CalcValue<PercentOr<Length>>, 2>{
                c.borders->radii.c,
                c.borders->radii.d,
            });
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Array<CalcValue<PercentOr<Length>>, 2> value;
            value[0] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            if (c.ended()) {
                value[1] = value[0];
            } else {
                value[1] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            }
            return Ok(makeRc<BorderRadiusTopRightProperty>(value));
        }
    };

    Array<CalcValue<PercentOr<Length>>, 2> _value;

    BorderRadiusTopRightProperty(Array<CalcValue<PercentOr<Length>>, 2> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().radii.c = _value[0];
        c.borders.cow().radii.d = _value[1];
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusTopLeftProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-top-left-radius"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRadiusTopLeftProperty>(makeArray<CalcValue<PercentOr<Length>>, 2>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRadiusTopLeftProperty>(Array<CalcValue<PercentOr<Length>>, 2>{
                c.borders->radii.a,
                c.borders->radii.b,
            });
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Array<CalcValue<PercentOr<Length>>, 2> value;
            value[0] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            eatWhitespace(c);
            if (c.ended()) {
                value[1] = value[0];
            } else {
                value[1] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            }
            return Ok(makeRc<BorderRadiusTopLeftProperty>(value));
        }
    };

    Array<CalcValue<PercentOr<Length>>, 2> _value;

    BorderRadiusTopLeftProperty(Array<CalcValue<PercentOr<Length>>, 2> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().radii.a = _value[1];
        c.borders.cow().radii.b = _value[0];
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusBottomRightProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-bottom-right-radius"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRadiusBottomRightProperty>(makeArray<CalcValue<PercentOr<Length>>, 2>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRadiusBottomRightProperty>(Array<CalcValue<PercentOr<Length>>, 2>{
                c.borders->radii.e,
                c.borders->radii.f,
            });
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Array<CalcValue<PercentOr<Length>>, 2> value;
            value[0] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            if (c.ended()) {
                value[1] = value[0];
            } else {
                value[1] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            }
            return Ok(makeRc<BorderRadiusBottomRightProperty>(value));
        }
    };

    Array<CalcValue<PercentOr<Length>>, 2> _value;

    BorderRadiusBottomRightProperty(Array<CalcValue<PercentOr<Length>>, 2> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().radii.e = _value[1];
        c.borders.cow().radii.f = _value[0];
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusBottomLeftProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-bottom-left-radius"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRadiusBottomLeftProperty>(makeArray<CalcValue<PercentOr<Length>>, 2>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRadiusBottomLeftProperty>(Array<CalcValue<PercentOr<Length>>, 2>{
                c.borders->radii.g,
                c.borders->radii.h,
            });
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Array<CalcValue<PercentOr<Length>>, 2> value;
            value[0] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            if (c.ended()) {
                value[1] = value[0];
            } else {
                value[1] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            }
            return Ok(makeRc<BorderRadiusBottomLeftProperty>(value));
        }
    };

    Array<CalcValue<PercentOr<Length>>, 2> _value;

    BorderRadiusBottomLeftProperty(Array<CalcValue<PercentOr<Length>>, 2> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().radii.g = _value[0];
        c.borders.cow().radii.h = _value[1];
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-radius"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRadiusProperty>(Math::Radii<CalcValue<PercentOr<Length>>>{CalcValue<PercentOr<Length>>(Length{})});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRadiusProperty>(c.borders->radii);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRadiusProperty>(try$(parseValue<Math::Radii<CalcValue<PercentOr<Length>>>>(c))));
        }
    };

    Math::Radii<CalcValue<PercentOr<Length>>> _value;

    BorderRadiusProperty(Math::Radii<CalcValue<PercentOr<Length>>> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().radii = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderTopProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-top"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderTopProperty>(Border{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderTopProperty>(c.borders->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Border value;
            while (not c.ended()) {
                auto width = parseValue<CalcValue<Length>>(c);
                if (width) {
                    value.width = width.unwrap();
                    continue;
                }

                auto color = parseValue<Color>(c);
                if (color) {
                    value.color = color.unwrap();
                    continue;
                }

                auto style = parseValue<Gfx::BorderStyle>(c);
                if (style) {
                    value.style = style.unwrap();
                    continue;
                }

                break;
            }
            return Ok(makeRc<BorderTopProperty>(value));
        }
    };

    Border _value;

    BorderTopProperty(Border value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().top = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderRightProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-right"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRightProperty>(Border{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRightProperty>(c.borders->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Border value;
            while (not c.ended()) {
                auto width = parseValue<CalcValue<Length>>(c);
                if (width) {
                    value.width = width.unwrap();
                    continue;
                }

                auto color = parseValue<Color>(c);
                if (color) {
                    value.color = color.unwrap();
                    continue;
                }

                auto style = parseValue<Gfx::BorderStyle>(c);
                if (style) {
                    value.style = style.unwrap();
                    continue;
                }

                break;
            }
            return Ok(makeRc<BorderRightProperty>(value));
        }
    };

    Border _value;

    BorderRightProperty(Border value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderBottomProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-bottom"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderBottomProperty>(Border{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderBottomProperty>(c.borders->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Border value;
            while (not c.ended()) {
                auto width = parseValue<CalcValue<Length>>(c);
                if (width) {
                    value.width = width.unwrap();
                    continue;
                }

                auto color = parseValue<Color>(c);
                if (color) {
                    value.color = color.unwrap();
                    continue;
                }

                auto style = parseValue<Gfx::BorderStyle>(c);
                if (style) {
                    value.style = style.unwrap();
                    continue;
                }

                break;
            }
            return Ok(makeRc<BorderBottomProperty>(value));
        }
    };

    Border _value;

    BorderBottomProperty(Border value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().bottom = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderLeftProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-left"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderLeftProperty>(Border{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderLeftProperty>(c.borders->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Border value;
            while (not c.ended()) {
                auto width = parseValue<CalcValue<Length>>(c);
                if (width) {
                    value.width = width.unwrap();
                    continue;
                }

                auto color = parseValue<Color>(c);
                if (color) {
                    value.color = color.unwrap();
                    continue;
                }

                auto style = parseValue<Gfx::BorderStyle>(c);
                if (style) {
                    value.style = style.unwrap();
                    continue;
                }

                break;
            }
            return Ok(makeRc<BorderLeftProperty>(value));
        }
    };

    Border _value;

    BorderLeftProperty(Border value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderProperty>(Border{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderProperty>(c.borders->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderProperty>(try$(parseValue<Border>(c))));
        }
    };

    Border _value;

    BorderProperty(Border value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().top = _value;
        c.borders.cow().bottom = _value;
        c.borders.cow().start = _value;
        c.borders.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderWidthProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-width"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderWidthProperty>(Math::Insets<LineWidth>{LineWidth{Keywords::MEDIUM}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderWidthProperty>(Math::Insets<LineWidth>{
                c.borders->start.width,
                c.borders->end.width,
                c.borders->top.width,
                c.borders->bottom.width,
            });
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderWidthProperty>(try$(parseValue<Math::Insets<LineWidth>>(c))));
        }
    };

    Math::Insets<LineWidth> _value;

    BorderWidthProperty(Math::Insets<LineWidth> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        auto& borders = c.borders.cow();
        borders.start.width = _value.start;
        borders.end.width = _value.end;
        borders.top.width = _value.top;
        borders.bottom.width = _value.bottom;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Content ---------------------------------------------------------------

// https://www.w3.org/TR/css-gcpm-3/
// https://drafts.csswg.org/css-content/#content-property
export struct ContentProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "content"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<ContentProperty>(Keywords::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ContentProperty>(c.content);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ContentProperty>(try$(parseValue<Content>(c))));
        }
    };

    Content _value;

    ContentProperty(Content value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.content = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Clip Path -------------------------------------------------------------

// https://drafts.fxtf.org/css-masking/#the-clip-path
export struct ClipPathProperty : Property {
    using Value = Union</* Url, */ BasicShape, Keywords::None>;

    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "clip-path"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<ClipPathProperty>(Keywords::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            if (c.clip->has())
                return makeRc<ClipPathProperty>(c.clip->unwrap());
            return makeRc<ClipPathProperty>(Keywords::NONE);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ClipPathProperty>(try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    ClipPathProperty(Value value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        if (auto clipShape = _value.is<BasicShape>())
            c.clip.cow() = *clipShape;
        else
            c.clip.cow() = NONE;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Borders - Table -------------------------------------------------------

// https://www.w3.org/TR/CSS22/tables.html#propdef-border-collapse
export struct BorderCollapseProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-collapse"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderCollapseProperty>(BorderCollapse::SEPARATE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderCollapseProperty>(c.table->collapse);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderCollapseProperty>(try$(parseValue<BorderCollapse>(c))));
        }
    };

    BorderCollapse _value;

    BorderCollapseProperty(BorderCollapse value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.table.cow().collapse = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/tables.html#propdef-border-spacing
export struct BorderSpacingProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "border-spacing"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderSpacingProperty>(BorderSpacing{0_au, 0_au});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderSpacingProperty>(c.table->spacing);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderSpacingProperty>(try$(parseValue<BorderSpacing>(c))));
        }
    };

    BorderSpacing _value;

    BorderSpacingProperty(BorderSpacing value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.table.cow().spacing = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Breaks ----------------------------------------------------------------

// https://www.w3.org/TR/css-break-3/#propdef-break-after
export struct BreakAfterProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "break-after"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BreakAfterProperty>(BreakBetween::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BreakAfterProperty>(c.break_->after);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BreakAfterProperty>(try$(parseValue<BreakBetween>(c))));
        }
    };

    BreakBetween _value;

    BreakAfterProperty(BreakBetween value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.break_.cow().after = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-break-3/#propdef-break-before
export struct BreakBeforeProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "break-before"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BreakBeforeProperty>(BreakBetween::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BreakBeforeProperty>(c.break_->before);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BreakBeforeProperty>(try$(parseValue<BreakBetween>(c))));
        }
    };

    BreakBetween _value;

    BreakBeforeProperty(BreakBetween value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.break_.cow().before = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-break-3/#break-within
export struct BreakInsideProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "break-inside"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BreakInsideProperty>(BreakInside::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BreakInsideProperty>(c.break_->inside);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BreakInsideProperty>(try$(parseValue<BreakInside>(c))));
        }
    };

    BreakInside _value;

    BreakInsideProperty(BreakInside value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.break_.cow().inside = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Flex ------------------------------------------------------------------

// https://www.w3.org/TR/css-flexbox-1/#flex-basis-property
export struct FlexBasisProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "flex-basis"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FlexBasisProperty>(Keywords::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexBasisProperty>(c.flex->basis);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexBasisProperty>(try$(parseValue<FlexBasis>(c))));
        }
    };

    FlexBasis _value;

    FlexBasisProperty(FlexBasis value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.flex.cow().basis = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-direction
export struct FlexDirectionProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "flex-direction"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FlexDirectionProperty>(FlexDirection::ROW);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexDirectionProperty>(c.flex->direction);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexDirectionProperty>(try$(parseValue<FlexDirection>(c))));
        }
    };

    FlexDirection _value;

    FlexDirectionProperty(FlexDirection value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.flex.cow().direction = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#flex-grow-property
export struct FlexGrowProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "flex-grow"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FlexGrowProperty>(Number{0});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexGrowProperty>(c.flex->grow);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexGrowProperty>(try$(parseValue<Number>(c))));
        }
    };

    Number _value;

    FlexGrowProperty(Number value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.flex.cow().grow = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-shrink
export struct FlexShrinkProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "flex-shrink"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FlexShrinkProperty>(Number{1});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexShrinkProperty>(c.flex->shrink);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexShrinkProperty>(try$(parseValue<Number>(c))));
        }
    };

    Number _value;

    FlexShrinkProperty(Number value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.flex.cow().shrink = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-wrap
export struct FlexWrapProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "flex-wrap"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FlexWrapProperty>(FlexWrap::NOWRAP);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexWrapProperty>(c.flex->wrap);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexWrapProperty>(try$(parseValue<FlexWrap>(c))));
        }
    };

    FlexWrap _value;

    FlexWrapProperty(FlexWrap value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.flex.cow().wrap = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-flow
export struct FlexFlowProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "flex-flow"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FlexFlowProperty>(Tuple<FlexDirection, FlexWrap>{FlexDirection::ROW, FlexWrap::NOWRAP});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexFlowProperty>(Tuple<FlexDirection, FlexWrap>{c.flex->direction, c.flex->wrap});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            Tuple<FlexDirection, FlexWrap> value{FlexDirection::ROW, FlexWrap::NOWRAP};

            auto direction = parseValue<FlexDirection>(c);
            if (direction) {
                value.v0 = direction.unwrap();

                auto wrap = parseValue<FlexWrap>(c);
                if (wrap)
                    value.v1 = wrap.unwrap();
            } else {
                auto wrap = parseValue<FlexWrap>(c);
                if (not wrap)
                    return Error::invalidData("expected flex direction or wrap");
                value.v1 = wrap.unwrap();

                direction = parseValue<FlexDirection>(c);
                if (direction)
                    value.v0 = direction.unwrap();
            }

            return Ok(makeRc<FlexFlowProperty>(value));
        }
    };

    Tuple<FlexDirection, FlexWrap> _value;

    FlexFlowProperty(Tuple<FlexDirection, FlexWrap> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.flex.cow().direction = _value.v0;
        c.flex.cow().wrap = _value.v1;
    }

    void repr(Io::Emit& e) const override {
        e("{} {}", _value.v0, _value.v1);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex
export struct FlexProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "flex"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FlexProperty>(FlexItemProps{Keywords::AUTO, 0, 1});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexProperty>(FlexItemProps{c.flex->basis, c.flex->grow, c.flex->shrink});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            FlexItemProps value{Keywords::AUTO, 0, 1};

            if (c.skip(Css::Token::ident("none"))) {
                value = {Keywords::AUTO, 0, 0};
                return Ok(makeRc<FlexProperty>(value));
            } else if (c.skip(Css::Token::ident("initial"))) {
                value = {Keywords::AUTO, 0, 1};
                return Ok(makeRc<FlexProperty>(value));
            }

            // deafult values if these parameters are omitted
            value.flexGrow = value.flexShrink = 1;
            value.flexBasis = CalcValue<PercentOr<Length>>(Length{});

            auto parseGrowShrink = [](Cursor<Css::Sst>& c, FlexItemProps& value) -> Res<> {
                auto grow = parseValue<Number>(c);
                if (not grow)
                    return Error::invalidData("expected flex item grow");

                value.flexGrow = grow.unwrap();

                auto shrink = parseValue<Number>(c);
                if (shrink)
                    value.flexShrink = shrink.unwrap();

                return Ok();
            };

            auto parsedGrowAndMaybeShrink = parseGrowShrink(c, value);
            if (parsedGrowAndMaybeShrink) {
                auto basis = parseValue<FlexBasis>(c);
                if (basis)
                    value.flexBasis = basis.unwrap();
            } else {
                auto basis = parseValue<FlexBasis>(c);
                if (basis)
                    value.flexBasis = basis.unwrap();
                else
                    return Error::invalidData("expected flex item grow or basis");

                auto parsedGrowAndMaybeShrink = parseGrowShrink(c, value);
            }
            return Ok(makeRc<FlexProperty>(value));
        }
    };

    FlexItemProps _value;

    FlexProperty(FlexItemProps value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        auto& flex = c.flex.cow();
        flex.basis = _value.flexBasis;
        flex.grow = _value.flexGrow;
        flex.shrink = _value.flexShrink;
    }

    void repr(Io::Emit& e) const override {
        e("{} {} {}", _value.flexGrow, _value.flexShrink, _value.flexBasis);
    }
};

// MARK: Float & Clear ---------------------------------------------------------

export struct FloatProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "float"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FloatProperty>(Float::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FloatProperty>(c.float_);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FloatProperty>(try$(parseValue<Float>(c))));
        }
    };

    Float _value;

    FloatProperty(Float value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.float_ = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct ClearProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "clear"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<ClearProperty>(Clear::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ClearProperty>(c.clear);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ClearProperty>(try$(parseValue<Clear>(c))));
        }
    };

    Clear _value;

    ClearProperty(Clear value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.clear = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Fonts -----------------------------------------------------------------

// https://www.w3.org/TR/css-fonts-4/#font-family-prop
export struct FontFamilyProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "font-family"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FontFamilyProperty>(Vec<FontFamily>{"sans-serif"_sym});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FontFamilyProperty>(c.font->families);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Vec<FontFamily> value{};
            eatWhitespace(c);
            while (not c.ended()) {
                value.pushBack(try$(parseValue<FontFamily>(c)));

                eatWhitespace(c);
                c.skip(Css::Token::comma());
                eatWhitespace(c);
            }
            return Ok(makeRc<FontFamilyProperty>(std::move(value)));
        }
    };

    Vec<FontFamily> _value;

    FontFamilyProperty(Vec<FontFamily> value) : _value(std::move(value)) {}

    void apply(SpecifiedValues& c) const override {
        c.font.cow().families = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-weight-prop
export struct FontWeightProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "font-weight"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FontWeightProperty>(FontWeight{Gfx::FontWeight::REGULAR});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FontWeightProperty>(c.font->weight);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontWeightProperty>(try$(parseValue<FontWeight>(c))));
        }
    };

    FontWeight _value;

    FontWeightProperty(FontWeight value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.font.cow().weight = _value.resolve();
    }

    void apply(SpecifiedValues const& parent, SpecifiedValues& c) const override {
        c.font.cow().weight = _value.resolve(parent.font->weight);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-width-prop
export struct FontWidthProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "font-width"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FontWidthProperty>(FontWidth::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FontWidthProperty>(c.font->width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontWidthProperty>(try$(parseValue<FontWidth>(c))));
        }
    };

    FontWidth _value;

    FontWidthProperty(FontWidth value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.font.cow().width = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-style-prop
export struct FontStyleProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "font-style"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FontStyleProperty>(FontStyle::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FontStyleProperty>(c.font->style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontStyleProperty>(try$(parseValue<FontStyle>(c))));
        }
    };

    FontStyle _value;

    FontStyleProperty(FontStyle value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.font.cow().style = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-prop
export struct FontProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "font"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FontProperty>(FontProps{}, None{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FontProperty>(*c.font, None{});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            // TODO: system family name
            FontProps value;
            Opt<FontWeight> unresolvedWeight;

            while (true) {
                auto fontStyle = parseValue<FontStyle>(c);
                if (fontStyle) {
                    value.style = fontStyle.unwrap();
                    continue;
                }

                auto fontWeight = parseValue<FontWeight>(c);
                if (fontWeight) {
                    unresolvedWeight = fontWeight.unwrap();
                    continue;
                }

                // TODO: font variant https://www.w3.org/TR/css-fonts-4/#font-variant-css21-values

                auto fontWidth = parseValue<FontWidth>(c);
                if (fontWidth) {
                    value.width = fontWidth.unwrap();
                    continue;
                }

                auto fontSize = parseValue<FontSize>(c);
                if (fontSize) {
                    value.size = fontSize.unwrap();
                    break;
                }

                return Error::invalidData("expected font-style, font-weight, font-width or font-size");
            }

            if (c.skip(Css::Token::delim("/"))) {
                auto lh = Ok(parseValue<LineHeight>(c));
                // TODO: use lineheight parsed value
            }

            value.families = {try$(parseValue<FontFamily>(c))};

            return Ok(makeRc<FontProperty>(std::move(value), unresolvedWeight));
        }
    };

    FontProps _value;
    Opt<FontWeight> _unresolvedWeight;

    FontProperty(FontProps value, Opt<FontWeight> unresolvedWeight)
        : _value(std::move(value)), _unresolvedWeight(unresolvedWeight) {}

    void apply(SpecifiedValues& c) const override {
        c.font.cow() = _value;
        if (_unresolvedWeight)
            c.font.cow().weight = _unresolvedWeight->resolve();
    }

    void apply(SpecifiedValues const& parent, SpecifiedValues& c) const override {
        c.font.cow() = _value;
        if (_unresolvedWeight)
            c.font.cow().weight = _unresolvedWeight->resolve(parent.font->weight);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-size-prop
export struct FontSizeProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "font-size"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FontSizeProperty>(FontSize::MEDIUM);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FontSizeProperty>(c.font->size);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontSizeProperty>(try$(parseValue<FontSize>(c))));
        }
    };

    FontSize _value;

    FontSizeProperty(FontSize value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.font.cow().size = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Line ------------------------------------------------------------------

export struct LineHeightProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "line-height"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<LineHeightProperty>(LineHeight::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const&) const override {
            return makeRc<LineHeightProperty>(LineHeight::NORMAL); // TODO
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<LineHeightProperty>(try$(parseValue<LineHeight>(c))));
        }
    };

    LineHeight _value;

    LineHeightProperty(LineHeight value) : _value(value) {}

    void apply(SpecifiedValues&) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Margin ----------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-margin

export struct MarginTopProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "margin-top"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginTopProperty>(CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginTopProperty>(c.margin->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginTopProperty>(try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginTopProperty(Width value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.margin.cow().top = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginRightProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "margin-right"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginRightProperty>(CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginRightProperty>(c.margin->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginRightProperty>(try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginRightProperty(Width value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.margin.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginBottomProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "margin-bottom"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginBottomProperty>(CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginBottomProperty>(c.margin->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBottomProperty>(try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginBottomProperty(Width value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.margin.cow().bottom = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginLeftProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "margin-left"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginLeftProperty>(CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginLeftProperty>(c.margin->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginLeftProperty>(try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginLeftProperty(Width value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.margin.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "margin"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginProperty>(Math::Insets<Width>{CalcValue<PercentOr<Length>>(Length{})});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginProperty>(*c.margin);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginProperty>(try$(parseValue<Math::Insets<Width>>(c))));
        }
    };

    Math::Insets<Width> _value;

    MarginProperty(Math::Insets<Width> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.margin.cow() = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#margin-properties

export struct MarginInlineStartProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "margin-inline-start"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginInlineStartProperty>(CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginInlineStartProperty>(c.margin->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginInlineStartProperty>(try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginInlineStartProperty(Width value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginInlineEndProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "margin-inline-end"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginInlineEndProperty>(CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginInlineEndProperty>(c.margin->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginInlineEndProperty>(try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginInlineEndProperty(Width value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginInlineProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "margin-inline"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginInlineProperty>(Math::Insets<Width>{CalcValue<PercentOr<Length>>(Length{})});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginInlineProperty>(Math::Insets<Width>{c.margin->start, c.margin->end});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginInlineProperty>(try$(parseValue<Math::Insets<Width>>(c))));
        }
    };

    Math::Insets<Width> _value;

    MarginInlineProperty(Math::Insets<Width> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().start = _value.start;
        c.margin.cow().end = _value.end;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginBlockStartProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "margin-block-start"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginBlockStartProperty>(CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginBlockStartProperty>(c.margin->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBlockStartProperty>(try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginBlockStartProperty(Width value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().top = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginBlockEndProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "margin-block-end"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginBlockEndProperty>(CalcValue<PercentOr<Length>>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginBlockEndProperty>(c.margin->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBlockEndProperty>(try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    MarginBlockEndProperty(Width value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().bottom = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct MarginBlockProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "margin-block"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginBlockProperty>(Math::Insets<Width>{CalcValue<PercentOr<Length>>(Length{})});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MarginBlockProperty>(Math::Insets<Width>{c.margin->top, c.margin->bottom});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBlockProperty>(try$(parseValue<Math::Insets<Width>>(c))));
        }
    };

    Math::Insets<Width> _value;

    MarginBlockProperty(Math::Insets<Width> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().top = _value.top;
        c.margin.cow().bottom = _value.bottom;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-color-4/#propdef-opacity
export struct OpacityProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "opacity"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<OpacityProperty>(Number{1});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OpacityProperty>(c.opacity);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto maybePercent = parseValue<Percent>(c);
            if (maybePercent) {
                return Ok(makeRc<OpacityProperty>(maybePercent.unwrap().value() / 100));
            } else {
                return Ok(makeRc<OpacityProperty>(try$(parseValue<Number>(c))));
            }
        }
    };

    Number _value;

    OpacityProperty(Number value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.opacity = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Outline --------------------------------------------------------------

// https://drafts.csswg.org/css-ui/#outline
export struct OutlineProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "outline"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<OutlineProperty>(Outline{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OutlineProperty>(*c.outline);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Outline value;
            bool styleSet = false;
            while (not c.ended()) {
                auto width = parseValue<CalcValue<Length>>(c);
                if (width) {
                    value.width = width.unwrap();
                    continue;
                }

                auto color = parseValue<Color>(c);
                if (color) {
                    value.color = color.unwrap();
                    continue;
                }

                auto style = parseValue<Gfx::BorderStyle>(c);
                if (style) {
                    value.style = style.unwrap();
                    styleSet = true;
                    continue;
                }

                if (c.skip(Css::Token::ident("auto"))) {
                    if (not styleSet)
                        value.style = Keywords::AUTO;
                    value.color = Keywords::AUTO;
                    continue;
                }

                break;
            }

            return Ok(makeRc<OutlineProperty>(value));
        }
    };

    Outline _value;

    OutlineProperty(Outline value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.outline.cow() = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-width
export struct OutlineWidthProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "outline-width"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<OutlineWidthProperty>(LineWidth{Keywords::MEDIUM});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OutlineWidthProperty>(c.outline->width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OutlineWidthProperty>(try$(parseValue<LineWidth>(c))));
        }
    };

    LineWidth _value;

    OutlineWidthProperty(LineWidth value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.outline.cow().width = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-style
export struct OutlineStyleProperty : Property {
    using Value = Union<Keywords::Auto, Gfx::BorderStyle>;

    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "outline-style"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<OutlineStyleProperty>(Value{Gfx::BorderStyle::NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OutlineStyleProperty>(c.outline->style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OutlineStyleProperty>(try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    OutlineStyleProperty(Value value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.outline.cow().style = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-color
export struct OutlineColorProperty : Property {
    using Value = Union<Keywords::Auto, Color>;

    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "outline-color"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<OutlineColorProperty>(Value{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OutlineColorProperty>(c.outline->color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OutlineColorProperty>(try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    OutlineColorProperty(Value value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.outline.cow().color = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-offset
export struct OutlineOffsetProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "outline-offset"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<OutlineOffsetProperty>(CalcValue<Length>{0_au});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OutlineOffsetProperty>(c.outline->offset);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OutlineOffsetProperty>(try$(parseValue<CalcValue<Length>>(c))));
        }
    };

    CalcValue<Length> _value;

    OutlineOffsetProperty(CalcValue<Length> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.outline.cow().offset = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Overflow --------------------------------------------------------------

// https://www.w3.org/TR/css-overflow/#overflow-control
export struct OverflowXProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "overflow-x"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<OverflowXProperty>(Overflow::VISIBLE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OverflowXProperty>(c.overflows.x);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OverflowXProperty>(try$(parseValue<Overflow>(c))));
        }
    };

    Overflow _value;

    OverflowXProperty(Overflow value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.overflows.x = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-control
export struct OverflowYProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "overflow-y"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<OverflowYProperty>(Overflow::VISIBLE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OverflowYProperty>(c.overflows.y);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OverflowYProperty>(try$(parseValue<Overflow>(c))));
        }
    };

    Overflow _value;

    OverflowYProperty(Overflow value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.overflows.y = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-block
export struct OverflowBlockProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "overflow-block"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<OverflowBlockProperty>(Overflow::VISIBLE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OverflowBlockProperty>(c.overflows.block);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OverflowBlockProperty>(try$(parseValue<Overflow>(c))));
        }
    };

    Overflow _value;

    OverflowBlockProperty(Overflow value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.overflows.block = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-inline
export struct OverflowInlineProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "overflow-inline"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<OverflowInlineProperty>(Overflow::VISIBLE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OverflowInlineProperty>(c.overflows.inline_);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OverflowInlineProperty>(try$(parseValue<Overflow>(c))));
        }
    };

    Overflow _value;

    OverflowInlineProperty(Overflow value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.overflows.inline_ = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow-3/#propdef-overflow
export struct OverflowProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "overflow"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<OverflowProperty>(Pair<Overflow>{Overflow::VISIBLE, Overflow::VISIBLE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OverflowProperty>(Pair<Overflow>{c.overflows.x, c.overflows.y});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            eatWhitespace(c);
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            Pair<Overflow> value;
            value.v0 = try$(parseValue<Overflow>(c));

            eatWhitespace(c);
            if (c.ended()) {
                value.v1 = value.v0;
            } else {
                value.v1 = try$(parseValue<Overflow>(c));
            }

            return Ok(makeRc<OverflowProperty>(value));
        }
    };

    Pair<Overflow> _value;

    OverflowProperty(Pair<Overflow> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.overflows.x = _value.v0;
        c.overflows.y = _value.v1;
    }

    void repr(Io::Emit& e) const override {
        e("{} {}", _value.v0, _value.v1);
    }
};

// MARK: Padding ---------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-padding

export struct PaddingTopProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "padding-top"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<PaddingTopProperty>(CalcValue<PercentOr<Length>>{Length{}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<PaddingTopProperty>(c.padding->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingTopProperty>(try$(parseValue<CalcValue<PercentOr<Length>>>(c))));
        }
    };

    CalcValue<PercentOr<Length>> _value;

    PaddingTopProperty(CalcValue<PercentOr<Length>> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.padding.cow().top = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingRightProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "padding-right"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<PaddingRightProperty>(CalcValue<PercentOr<Length>>{Length{}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<PaddingRightProperty>(c.padding->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingRightProperty>(try$(parseValue<CalcValue<PercentOr<Length>>>(c))));
        }
    };

    CalcValue<PercentOr<Length>> _value;

    PaddingRightProperty(CalcValue<PercentOr<Length>> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.padding.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingBottomProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "padding-bottom"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<PaddingBottomProperty>(CalcValue<PercentOr<Length>>{Length{}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<PaddingBottomProperty>(c.padding->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingBottomProperty>(try$(parseValue<CalcValue<PercentOr<Length>>>(c))));
        }
    };

    CalcValue<PercentOr<Length>> _value;

    PaddingBottomProperty(CalcValue<PercentOr<Length>> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.padding.cow().bottom = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingLeftProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "padding-left"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<PaddingLeftProperty>(CalcValue<PercentOr<Length>>{Length{}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<PaddingLeftProperty>(c.padding->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingLeftProperty>(try$(parseValue<CalcValue<PercentOr<Length>>>(c))));
        }
    };

    CalcValue<PercentOr<Length>> _value;

    PaddingLeftProperty(CalcValue<PercentOr<Length>> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.padding.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingInlineStartProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "padding-inline-start"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<PaddingInlineStartProperty>(CalcValue<PercentOr<Length>>{Length{}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<PaddingInlineStartProperty>(c.padding->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingInlineStartProperty>(try$(parseValue<CalcValue<PercentOr<Length>>>(c))));
        }
    };

    CalcValue<PercentOr<Length>> _value;

    PaddingInlineStartProperty(CalcValue<PercentOr<Length>> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.padding.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingInlineEndProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "padding-inline-end"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<PaddingInlineEndProperty>(CalcValue<PercentOr<Length>>{Length{}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<PaddingInlineEndProperty>(c.padding->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingInlineEndProperty>(try$(parseValue<CalcValue<PercentOr<Length>>>(c))));
        }
    };

    CalcValue<PercentOr<Length>> _value;

    PaddingInlineEndProperty(CalcValue<PercentOr<Length>> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.padding.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "padding"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<PaddingProperty>(Math::Insets<CalcValue<PercentOr<Length>>>{Length{}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<PaddingProperty>(*c.padding);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingProperty>(try$(parseValue<Math::Insets<CalcValue<PercentOr<Length>>>>(c))));
        }
    };

    Math::Insets<CalcValue<PercentOr<Length>>> _value;

    PaddingProperty(Math::Insets<CalcValue<PercentOr<Length>>> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.padding.cow() = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-display-3/#order-property
export struct OrderProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "order"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<OrderProperty>(Integer{0});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OrderProperty>(c.order);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OrderProperty>(try$(parseValue<Integer>(c))));
        }
    };

    Integer _value;

    OrderProperty(Integer value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.order = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Positioning -----------------------------------------------------------

// https://www.w3.org/TR/CSS22/visuren.html#positioning-scheme
export struct PositionProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "position"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<PositionProperty>(Position{Keywords::STATIC});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<PositionProperty>(c.position);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PositionProperty>(try$(parseValue<Position>(c))));
        }
    };

    Position _value;

    PositionProperty(Position value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.position = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-top
export struct TopProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "top"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<TopProperty>(Width{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<TopProperty>(c.offsets->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TopProperty>(try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    TopProperty(Width value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.offsets.cow().top = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-right
export struct RightProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "right"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<RightProperty>(Width{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<RightProperty>(c.offsets->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<RightProperty>(try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    RightProperty(Width value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.offsets.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-bottom
export struct BottomProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "bottom"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BottomProperty>(Width{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BottomProperty>(c.offsets->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BottomProperty>(try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    BottomProperty(Width value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.offsets.cow().bottom = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-left
export struct LeftProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "left"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<LeftProperty>(Width{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<LeftProperty>(c.offsets->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<LeftProperty>(try$(parseValue<Width>(c))));
        }
    };

    Width _value;

    LeftProperty(Width value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.offsets.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Sizing ----------------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export struct BoxSizingProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "box-sizing"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<BoxSizingProperty>(BoxSizing::CONTENT_BOX);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BoxSizingProperty>(c.boxSizing);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            BoxSizing value;
            if (c.skip(Css::Token::ident("border-box")))
                value = BoxSizing::BORDER_BOX;
            else if (c.skip(Css::Token::ident("content-box")))
                value = BoxSizing::CONTENT_BOX;
            else
                return Error::invalidData("expected 'border-box' or 'content-box'");

            return Ok(makeRc<BoxSizingProperty>(value));
        }
    };

    BoxSizing _value;

    BoxSizingProperty(BoxSizing value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.boxSizing = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width

export struct WidthProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "width"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<WidthProperty>(Size{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<WidthProperty>(c.sizing->width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<WidthProperty>(try$(parseValue<Size>(c))));
        }
    };

    Size _value;

    WidthProperty(Size value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.sizing.cow().width = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-height

export struct HeightProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "height"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<HeightProperty>(Size{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<HeightProperty>(c.sizing->height);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<HeightProperty>(try$(parseValue<Size>(c))));
        }
    };

    Size _value;

    HeightProperty(Size value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.sizing.cow().height = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-width

export struct MinWidthProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "min-width"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MinWidthProperty>(Size{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MinWidthProperty>(c.sizing->minWidth);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MinWidthProperty>(try$(parseValue<Size>(c))));
        }
    };

    Size _value;

    MinWidthProperty(Size value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.sizing.cow().minWidth = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-height

export struct MinHeightProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "min-height"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MinHeightProperty>(Size{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MinHeightProperty>(c.sizing->minHeight);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MinHeightProperty>(try$(parseValue<Size>(c))));
        }
    };

    Size _value;

    MinHeightProperty(Size value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.sizing.cow().minHeight = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-width

export struct MaxWidthProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "max-width"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MaxWidthProperty>(MaxSize{Keywords::NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MaxWidthProperty>(c.sizing->maxWidth);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MaxWidthProperty>(try$(parseValue<MaxSize>(c))));
        }
    };

    MaxSize _value;

    MaxWidthProperty(MaxSize value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.sizing.cow().maxWidth = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-height

export struct MaxHeightProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "max-height"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<MaxHeightProperty>(MaxSize{Keywords::NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<MaxHeightProperty>(c.sizing->maxHeight);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MaxHeightProperty>(try$(parseValue<MaxSize>(c))));
        }
    };

    MaxSize _value;

    MaxHeightProperty(MaxSize value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.sizing.cow().maxHeight = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Text
// https://drafts.csswg.org/css-text-4

// https://drafts.csswg.org/css-text/#text-align-property

export struct TextAlignProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "text-align"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<TextAlignProperty>(TextAlign::LEFT);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<TextAlignProperty>(c.text->align);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            TextAlign value;
            if (c.skip(Css::Token::ident("left"))) {
                value = TextAlign::LEFT;
            } else if (c.skip(Css::Token::ident("right"))) {
                value = TextAlign::RIGHT;
            } else if (c.skip(Css::Token::ident("center"))) {
                value = TextAlign::CENTER;
            } else if (c.skip(Css::Token::ident("justify"))) {
                value = TextAlign::JUSTIFY;
            } else if (c.skip(Css::Token::ident("start"))) {
                value = TextAlign::START;
            } else if (c.skip(Css::Token::ident("end"))) {
                value = TextAlign::END;
            } else {
                return Error::invalidData("expected text-align");
            }
            return Ok(makeRc<TextAlignProperty>(value));
        }
    };

    TextAlign _value;

    TextAlignProperty(TextAlign value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.text.cow().align = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-text-4/#text-transform-property

export struct TextTransformProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "text-transform"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<TextTransformProperty>(TextTransform::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<TextTransformProperty>(c.text->transform);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            TextTransform value;
            if (c.skip(Css::Token::ident("none"))) {
                value = TextTransform::NONE;
            } else if (c.skip(Css::Token::ident("uppercase"))) {
                value = TextTransform::UPPERCASE;
            } else if (c.skip(Css::Token::ident("lowsercase"))) {
                value = TextTransform::LOWERCASE;
            } else {
                return Error::invalidData("expected text-transform");
            }

            return Ok(makeRc<TextTransformProperty>(value));
        }
    };

    TextTransform _value;

    TextTransformProperty(TextTransform value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.text.cow().transform = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Transform -------------------------------------------------------------
// https://drafts.csswg.org/css-transforms/#transform-property

// https://drafts.csswg.org/css-transforms/#transform-origin-property
export struct TransformOriginProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "transform-origin"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<TransformOriginProperty>(TransformOrigin{
                .xOffset = CalcValue<PercentOr<Length>>{Percent{50}},
                .yOffset = CalcValue<PercentOr<Length>>{Percent{50}},
            });
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<TransformOriginProperty>(c.transform->origin);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TransformOriginProperty>(try$(parseValue<TransformOrigin>(c))));
        }
    };

    TransformOrigin _value;

    TransformOriginProperty(TransformOrigin value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.transform.cow().origin = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-transforms/#transform-box
export struct TransformBoxProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "transform-box"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<TransformBoxProperty>(TransformBox{Keywords::VIEW_BOX});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<TransformBoxProperty>(c.transform->box);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TransformBoxProperty>(try$(parseValue<TransformBox>(c))));
        }
    };

    TransformBox _value;

    TransformBoxProperty(TransformBox value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.transform.cow().box = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-transforms/#propdef-transform
export struct TransformProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "transform"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<TransformProperty>(Transform{Keywords::NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<TransformProperty>(c.transform->transform);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TransformProperty>(try$(parseValue<Transform>(c))));
        }
    };

    Transform _value;

    TransformProperty(Transform value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.transform.cow().transform = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-display/#visibility
export struct VisibilityProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "visibility"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<VisibilityProperty>(Visibility::VISIBLE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<VisibilityProperty>(c.visibility);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Visibility value;
            if (c.skip(Css::Token::ident("visible"))) {
                value = Visibility::VISIBLE;
            } else if (c.skip(Css::Token::ident("hidden"))) {
                value = Visibility::HIDDEN;
            } else if (c.skip(Css::Token::ident("collapse"))) {
                value = Visibility::COLLAPSE;
            } else {
                return Error::invalidData("expected visibility");
            }

            return Ok(makeRc<VisibilityProperty>(value));
        }
    };

    Visibility _value;

    VisibilityProperty(Visibility value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.visibility = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-text/#white-space-property

export struct WhiteSpaceProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "white-space"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<WhiteSpaceProperty>(WhiteSpace::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<WhiteSpaceProperty>(c.text->whiteSpace);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            WhiteSpace value;
            if (c.skip(Css::Token::ident("normal"))) {
                value = WhiteSpace::NORMAL;
            } else if (c.skip(Css::Token::ident("nowrap"))) {
                value = WhiteSpace::NOWRAP;
            } else if (c.skip(Css::Token::ident("pre"))) {
                value = WhiteSpace::PRE;
            } else if (c.skip(Css::Token::ident("pre-wrap"))) {
                value = WhiteSpace::PRE_WRAP;
            } else if (c.skip(Css::Token::ident("pre-line"))) {
                value = WhiteSpace::PRE_LINE;
            } else if (c.skip(Css::Token::ident("break-spaces"))) {
                value = WhiteSpace::BREAK_SPACES;
            } else {
                return Error::invalidData("expected white-space");
            }

            return Ok(makeRc<WhiteSpaceProperty>(value));
        }
    };

    WhiteSpace _value;

    WhiteSpaceProperty(WhiteSpace value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.text.cow().whiteSpace = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css2/#z-index

export struct ZIndexProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "z-index"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<ZIndexProperty>(ZIndex{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ZIndexProperty>(c.zIndex);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ZIndexProperty>(try$(parseValue<ZIndex>(c))));
        }
    };

    ZIndex _value;

    ZIndexProperty(ZIndex value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.zIndex = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: SVG ----------------------------------------------------------------

// https://svgwg.org/svg2-draft/geometry.html#XProperty
export struct SVGXProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "x"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<SVGXProperty>(PercentOr<Length>{Length{0_au}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SVGXProperty>(c.svg->x);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SVGXProperty>(try$(parseValue<PercentOr<Length>>(c))));
        }
    };

    PercentOr<Length> _value;

    SVGXProperty(PercentOr<Length> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().x = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#YProperty
export struct SVGYProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "y"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<SVGYProperty>(PercentOr<Length>{Length{0_au}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SVGYProperty>(c.svg->y);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SVGYProperty>(try$(parseValue<PercentOr<Length>>(c))));
        }
    };

    PercentOr<Length> _value;

    SVGYProperty(PercentOr<Length> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().y = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#CXProperty
export struct SVGCXProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "cx"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<SVGCXProperty>(PercentOr<Length>{Length{0_au}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SVGCXProperty>(c.svg->cx);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SVGCXProperty>(try$(parseValue<PercentOr<Length>>(c))));
        }
    };

    PercentOr<Length> _value;

    SVGCXProperty(PercentOr<Length> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().cx = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#CYProperty
export struct SVGCYProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "cy"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<SVGCYProperty>(PercentOr<Length>{Length{0_au}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SVGCYProperty>(c.svg->cy);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SVGCYProperty>(try$(parseValue<PercentOr<Length>>(c))));
        }
    };

    PercentOr<Length> _value;

    SVGCYProperty(PercentOr<Length> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().cy = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#RProperty
export struct SVGRProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "r"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<SVGRProperty>(PercentOr<Length>{Length{0_au}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SVGRProperty>(c.svg->r);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SVGRProperty>(try$(parseValue<PercentOr<Length>>(c))));
        }
    };

    PercentOr<Length> _value;

    SVGRProperty(PercentOr<Length> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().r = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#FillProperty
export struct SVGFillProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "fill"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<SVGFillProperty>(Paint{Color{Gfx::BLACK}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SVGFillProperty>(c.svg->fill);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SVGFillProperty>(try$(parseValue<Paint>(c))));
        }
    };

    Paint _value;

    SVGFillProperty(Paint value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().fill = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/paths.html#TheDProperty
export struct SVGDProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "d"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<SVGDProperty>(Union<String, None>{NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SVGDProperty>(c.svg->d);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            eatWhitespace(c);
            if (c.peek() != Css::Sst::FUNC or c.peek().prefix != Css::Token::function("path(")) {
                return Error::invalidData("expected path function");
            }

            auto pathFunc = c.next();
            Cursor<Css::Sst> scanPath{pathFunc.content};
            return Ok(makeRc<SVGDProperty>(try$(parseValue<String>(scanPath))));
        }
    };

    Union<String, None> _value;

    SVGDProperty(Union<String, None> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().d = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/coords.html#ViewBoxAttribute
export struct SVGViewBoxProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "viewBox"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<SVGViewBoxProperty>(Opt<ViewBox>{NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SVGViewBoxProperty>(c.svg->viewBox);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            ViewBox viewBox;

            viewBox.minX = try$(parseValue<Number>(c));

            c.skip(Css::Token::comma());
            viewBox.minY = try$(parseValue<Number>(c));

            c.skip(Css::Token::comma());
            viewBox.width = try$(parseValue<Number>(c));

            c.skip(Css::Token::comma());
            viewBox.height = try$(parseValue<Number>(c));

            return Ok(makeRc<SVGViewBoxProperty>(Opt<ViewBox>{std::move(viewBox)}));
        }
    };

    Opt<ViewBox> _value;

    SVGViewBoxProperty(Opt<ViewBox> value) : _value(std::move(value)) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().viewBox = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#SpecifyingStrokePaint
export struct SVGStrokeProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "stroke"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<SVGStrokeProperty>(Paint{NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SVGStrokeProperty>(c.svg->stroke);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SVGStrokeProperty>(try$(parseValue<Paint>(c))));
        }
    };

    Paint _value;

    SVGStrokeProperty(Paint value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().stroke = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#StrokeOpacity
export struct SvgStrokeOpacityProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "stroke-opacity"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<SvgStrokeOpacityProperty>(Number{1});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SvgStrokeOpacityProperty>(c.svg->strokeOpacity);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto maybePercent = parseValue<Percent>(c);
            if (maybePercent) {
                return Ok(makeRc<SvgStrokeOpacityProperty>(maybePercent.unwrap().value() / 100));
            } else {
                return Ok(makeRc<SvgStrokeOpacityProperty>(try$(parseValue<Number>(c))));
            }
        }
    };

    Number _value;

    SvgStrokeOpacityProperty(Number value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().strokeOpacity = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#FillOpacity
export struct FillOpacityProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "fill-opacity"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<FillOpacityProperty>(Number{1});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FillOpacityProperty>(c.svg->fillOpacity);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto maybePercent = parseValue<Percent>(c);
            if (maybePercent) {
                return Ok(makeRc<FillOpacityProperty>(maybePercent.unwrap().value() / 100));
            } else {
                return Ok(makeRc<FillOpacityProperty>(try$(parseValue<Number>(c))));
            }
        }
    };

    Number _value;

    FillOpacityProperty(Number value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().fillOpacity = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#StrokeWidth
export struct StrokeWidthProperty : Property {
    struct Declaration : Property::Declaration {
        Symbol name() const override {
            return "stroke-width"_sym;
        }

        Rc<Property> initial() const override {
            return makeRc<StrokeWidthProperty>(PercentOr<Length>{Length{1_au}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<StrokeWidthProperty>(c.svg->strokeWidth);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<StrokeWidthProperty>(try$(parseValue<PercentOr<Length>>(c))));
        }
    };

    PercentOr<Length> _value;

    StrokeWidthProperty(PercentOr<Length> value) : _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().strokeWidth = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: OTHER -----------------------------------------------------------------
// These are no specs or behave differently than the others, you can find more details for each one in the comments.

// https://drafts.csswg.org/css-variables/#defining-variables
// this symbolizes a custom property, it starts with `--` and can be used to store a value that can be reused in the stylesheet
export struct CustomProp {
    Symbol varName;
    Css::Content value;

    CustomProp(Symbol varName, Css::Content value)
        : varName(varName), value(value) {
    }

    static constexpr Str name() { return "custom prop"; }

    void apply(SpecifiedValues& c) const {
        c.setCustomProp(varName, value);
    }

    void repr(Io::Emit& e) const {
        e("(var {#} = {})", varName, value);
    }
};

// NOTE: A property that could not be parsed, it's used to store the value
//       as-is and apply it with the cascade and custom properties
export struct DeferredProp {
    Symbol propName;
    Css::Content value;

    static constexpr Str name() { return "deferred prop"; }

    static bool _expandVariable(Cursor<Css::Sst>& c, Map<Symbol, Css::Content> const& env, Css::Content& out);

    static bool _expandFunction(Cursor<Css::Sst>& c, Map<Symbol, Css::Content> const& env, Css::Content& out);

    static void _expandContent(Cursor<Css::Sst>& c, Map<Symbol, Css::Content> const& env, Css::Content& out);

    void apply(SpecifiedValues const& parent, SpecifiedValues& c) const;

    void repr(Io::Emit& e) const {
        e("(deferred {#} = {})", propName, value);
    }
};

enum struct Default {
    INITIAL, //< represents the value defined as the property’s initial value.
    INHERIT, //< represents the property’s computed value on the parent element.
    UNSET,   //< acts as either inherit or initial, depending on whether the property is inherited or not.
    REVERT,  //< rolls back the cascade to the cascaded value of the earlier origin.

    _LEN,
};

export struct DefaultedProp {
    String propName;
    Default value;

    static constexpr Str name() { return "defaulted prop"; }

    void apply(SpecifiedValues const& parent, SpecifiedValues& c) const;

    void repr(Io::Emit&) const;
};

// MARK: Style Property  -------------------------------------------------------

using _StyleProp = Union<
    // Align
    AlignContentProp,
    JustifyContentProp,
    JustifySelfProp,
    AlignSelfProp,
    JustifyItemsProp,
    AlignItemsProp,

    RowGapProp,
    ColumnGapProp,

    // Baseline
    DominantBaselineProp,
    BaselineSourceProp,
    AlignmentBaselineProp,

    // Background
    BackgroundAttachmentProp,
    BackgroundColorProp,
    BackgroundImageProp,
    BackgroundPositionProp,
    BackgroundRepeatProp,
    BackgroundProp,
    ColorProp,
    DisplayProp,
    TableLayoutProp,
    CaptionSideProp,

    // Transform
    TransformOriginProp,
    TransformBoxProp,
    TransformProp,

    // Visibility
    VisibilityProp,

    // Borders
    BorderTopColorProp,
    BorderRightColorProp,
    BorderBottomColorProp,
    BorderLeftColorProp,
    BorderColorProp,

    BorderTopWidthProp,
    BorderRightWidthProp,
    BorderBottomWidthProp,
    BorderLeftWidthProp,

    BorderStyle,
    BorderTopStyleProp,
    BorderRightStyleProp,
    BorderBottomStyleProp,
    BorderLeftStyleProp,

    BorderRadiusTopRight,
    BorderRadiusTopLeft,
    BorderRadiusBottomRight,
    BorderRadiusBottomLeft,
    BorderRadius,

    BorderTopProp,
    BorderRightProp,
    BorderBottomProp,
    BorderLeftProp,
    BorderProp,

    BorderWidthProp,

    // Borders - Table
    BorderCollapseProp,
    BorderSpacingProp,

    // Clip
    ClipPathProp,

    // Content
    ContentProp,

    // Breaks
    BreakAfterProp,
    BreakBeforeProp,
    BreakInsideProp,

    // Flex
    FlexBasisProp,
    FlexDirectionProp,
    FlexGrowProp,
    FlexShrinkProp,
    FlexWrapProp,
    FlexFlowProp,
    FlexProp,

    // Float & Clear
    FloatProp,
    ClearProp,

    // Font
    FontFamilyProp,
    FontWeightProp,
    FontWidthProp,
    FontStyleProp,
    FontSizeProp,
    FontProp,

    // Line
    LineHeightProp,

    // Margin
    MarginTopProp,
    MarginRightProp,
    MarginBottomProp,
    MarginLeftProp,
    MarginProp,

    MarginInlineStartProp,
    MarginInlineEndProp,
    MarginInlineProp,

    MarginBlockStartProp,
    MarginBlockEndProp,
    MarginBlockProp,

    // Outline
    OutlineProp,
    OutlineColorProp,
    OutlineOffsetProp,
    OutlineStyleProp,
    OutlineWidthProp,

    // Overflow
    OverflowXProp,
    OverflowYProp,
    OverflowBlockProp,
    OverflowInlineProp,
    OverflowProp,

    OpacityProp,

    // Padding
    PaddingTopProp,
    PaddingRightProp,
    PaddingBottomProp,
    PaddingLeftProp,
    PaddingInlineStart,
    PaddingInlineEnd,
    PaddingProp,

    // Positioning
    PositionProp,
    TopProp,
    RightProp,
    BottomProp,
    LeftProp,

    // Sizing
    BoxSizingProp,
    WidthProp,
    HeightProp,
    MinWidthProp,
    MinHeightProp,
    MaxWidthProp,
    MaxHeightProp,

    // Text
    TextAlignProp,
    TextTransformProp,
    WhiteSpaceProp,

    // ZIndex
    ZIndexProp,

    // Other
    CustomProp,
    DeferredProp,
    DefaultedProp,

    // SVG
    SVGXProp,
    SVGYProp,
    SVGCXProp,
    SVGCYProp,
    SVGRProp,
    SVGFillProp,
    SVGDProp,
    SVGStrokeProp,
    SvgStrokeOpacityProp,
    SVGViewBoxProp,
    FillOpacityProp,
    StrokeWidthProp
    /**/
    >;

// FIXME: should be targeted in style computing refactoring
using SVGStyleProp = Union<
    SVGXProp,
    SVGYProp,
    SVGFillProp,
    SVGDProp,
    SVGCXProp,
    SVGCYProp,
    SVGRProp,
    SVGStrokeProp,
    SVGViewBoxProp,
    HeightProp,
    FillOpacityProp,
    WidthProp,
    StrokeWidthProp,
    TransformProp,
    TransformOriginProp>;

export enum struct Important {
    NO,
    YES,
};

export struct StyleProp : _StyleProp {
    using _StyleProp::_StyleProp;
    Important important = Important::NO;

    static constexpr Array LEGACY_ALIAS = {
        // https://drafts.csswg.org/css-align-3/#gap-legacy
        Pair<Str>{"grid-row-gap", "row-gap"},
        Pair<Str>{"grid-column-gap", "column-gap"},
        Pair<Str>{"grid-gap", "gap"},
    };

    Str name() const;

    void inherit(SpecifiedValues const& parent, SpecifiedValues& child) const;

    void apply(SpecifiedValues const& parent, SpecifiedValues& c) const;

    void repr(Io::Emit& e) const;
};

} // namespace Vaev::Style
