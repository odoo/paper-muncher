export module Vaev.Engine:style.select.pseudoClass;

import Karm.Core;
import Karm.Gc;

import :dom.element;
import :dom.names;
import :dom.text;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

export enum struct PseudoClass : u8 {
#define PSEUDO(ID, ...) ID,
#include "defs/pseudo-class.inc"

#undef PSEUDO
    _LEN,
};

export Opt<PseudoClass> parsePseudoClass(Str name) {
#define PSEUDO(IDENT, NAME) \
    if (eqCi(name, NAME))   \
        return Some(PseudoClass::IDENT);
#include "defs/pseudo-class.inc"

#undef PSEUDO

    return NONE;
}

export Str serializePseudoClass(PseudoClass pseudoClass) {
    switch (pseudoClass) {
#define PSEUDO(IDENT, NAME)  \
    case PseudoClass::IDENT: \
        return NAME;

#include "defs/pseudo-class.inc"

#undef PSEUDO
    default:
        unreachable();
    }
}

// MARK: Helpers ---------------------------------------------------------------

static Gc::Ptr<Dom::Element> _nearestAncestor(Dom::Element const& el, Dom::QualifiedName const& name) {
    for (auto curr = el.parentElement(); curr; curr = curr->parentElement())
        if (curr->qualifiedName == name)
            return curr;
    return nullptr;
}

// https://html.spec.whatwg.org/multipage/input.html#attr-input-type
static Str _inputType(Dom::Element const& el) {
    static constexpr Array TYPES = {
        "hidden"s, "text"s, "search"s, "tel"s, "url"s, "email"s, "password"s, "date"s, "month"s, "week"s,
        "time"s, "datetime-local"s, "number"s, "range"s, "color"s, "checkbox"s, "radio"s, "file"s,
        "submit"s, "image"s, "reset"s, "button"s
    };

    if (auto type = el.getAttribute(Html::TYPE_ATTR))
        for (auto known : TYPES)
            if (eqCi(*type, known))
                return known;
    return "text"s;
}

static bool _matchInput(Dom::Element const& el, auto... types) {
    if (el.qualifiedName != Html::INPUT_TAG)
        return false;
    auto type = _inputType(el);
    return ((type == types) or ...);
}

static bool _matchTextLikeInput(Dom::Element const& el) {
    return _matchInput(el, "text"s, "search"s, "url"s, "tel"s, "email"s, "password"s, "number"s);
}

static bool _matchDateTimeInput(Dom::Element const& el) {
    return _matchInput(el, "date"s, "month"s, "week"s, "time"s, "datetime-local"s);
}

static bool _matchRequirableInput(Dom::Element const& el) {
    return _matchTextLikeInput(el) or _matchDateTimeInput(el) or _matchInput(el, "checkbox"s, "radio"s, "file"s);
}

// MARK: Disabled --------------------------------------------------------------

// https://html.spec.whatwg.org/multipage/form-elements.html#concept-fe-disabled
static bool _matchInDisabledFieldset(Dom::Element const& el) {
    Dom::Element const* child = &el;
    for (auto curr = el.parentElement(); curr; child = &*curr, curr = curr->parentElement()) {
        if (curr->qualifiedName != Html::FIELDSET_TAG or not curr->hasAttribute(Html::DISABLED_ATTR))
            continue;

        Dom::Element const* legend = nullptr;
        for (auto c : curr->iterChildrenOfType<Dom::Element>()) {
            if (c->qualifiedName == Html::LEGEND_TAG) {
                legend = &*c;
                break;
            }
        }

        if (legend != child)
            return true;
    }
    return false;
}

// https://html.spec.whatwg.org/multipage/form-control-infrastructure.html#concept-fe-disabled
static bool _matchDisabledFormControl(Dom::Element const& el) {
    return Html::isDisableableFormControl(el.qualifiedName) and
           (el.hasAttribute(Html::DISABLED_ATTR) or _matchInDisabledFieldset(el));
}

// https://html.spec.whatwg.org/multipage/semantics-other.html#concept-element-disabled
static bool _matchActuallyDisabled(Dom::Element const& el) {
    // An element is said to be actually disabled if it is one of the following:
    // - a button element that is disabled
    // - an input element that is disabled
    // - a select element that is disabled
    // - a textarea element that is disabled
    if (_matchDisabledFormControl(el))
        return true;

    auto isAncestorSelectDisabled = [&] {
        auto select = _nearestAncestor(el, Html::SELECT_TAG);
        return select and _matchDisabledFormControl(*select);
    };

    // - an optgroup element that has a disabled attribute or whose nearest ancestor select is disabled
    if (el.qualifiedName == Html::OPTGROUP_TAG)
        return el.hasAttribute(Html::DISABLED_ATTR) or isAncestorSelectDisabled();

    // - an option element that is disabled or whose nearest ancestor select is disabled
    if (el.qualifiedName == Html::OPTION_TAG) {
        // https://html.spec.whatwg.org/multipage/form-elements.html#concept-option-disabled
        auto parent = el.parentElement();
        bool isDisabled = el.hasAttribute(Html::DISABLED_ATTR) or
                          (parent and parent->qualifiedName == Html::OPTGROUP_TAG and parent->hasAttribute(Html::DISABLED_ATTR));
        return isDisabled or isAncestorSelectDisabled();
    }

    // - a fieldset element that is a disabled fieldset
    if (el.qualifiedName == Html::FIELDSET_TAG)
        return el.hasAttribute(Html::DISABLED_ATTR) or _matchInDisabledFieldset(el);

    return false;
}

// MARK: Form State ------------------------------------------------------------

// https://html.spec.whatwg.org/multipage/form-elements.html#concept-button
static bool _matchSubmitButton(Dom::Element const& el) {
    if (el.qualifiedName == Html::BUTTON_TAG) {
        auto type = el.getAttribute(Html::TYPE_ATTR);
        return not type or not(eqCi(*type, "reset"s) or eqCi(*type, "button"s));
    }
    return _matchInput(el, "submit"s, "image"s);
}

// https://html.spec.whatwg.org/multipage/form-control-infrastructure.html#default-button
static bool _matchDefaultButton(Dom::Element const& el) {
    // A form element's default button is the first submit button in tree order whose form owner is that form element.
    // FIXME: Take the form attribute into account when computing the form owner.
    auto form = _nearestAncestor(el, Html::FORM_TAG);
    if (not _matchSubmitButton(el) or not form)
        return false;

    for (auto node : form->iterDepthFirst())
        if (auto curr = node->as<Dom::Element>(); curr and _matchSubmitButton(*curr))
            return &*curr == &el;
    return false;
}

// https://html.spec.whatwg.org/multipage/input.html#radio-button-group
static bool _matchRadioGroupChecked(Dom::Element const& el) {
    auto name = el.getAttribute(Html::NAME_ATTR);
    if (not name or name->len() == 0)
        return el.hasAttribute(Html::CHECKED_ATTR);

    auto form = _nearestAncestor(el, Html::FORM_TAG);
    Dom::Node const* root = &el;
    if (form)
        root = &*form;
    else
        while (auto parent = root->parentNode())
            root = &*parent;

    for (auto node : root->iterDepthFirst()) {
        auto curr = node->as<Dom::Element>();
        if (curr and _matchInput(*curr, "radio"s) and curr->getAttribute(Html::NAME_ATTR) == name and
            _nearestAncestor(*curr, Html::FORM_TAG) == form and curr->hasAttribute(Html::CHECKED_ATTR))
            return true;
    }
    return false;
}

// https://html.spec.whatwg.org/multipage/interaction.html#editing-host
static bool _matchEditable(Dom::Element const& el) {
    if (auto value = el.getAttribute(Html::CONTENTEDITABLE_ATTR)) {
        if (eqCi(*value, ""s) or eqCi(*value, "true"s) or eqCi(*value, "plaintext-only"s))
            return true;
        if (eqCi(*value, "false"s))
            return false;
    }
    auto parent = el.parentElement();
    return parent and _matchEditable(*parent);
}

static bool _matchReadWrite(Dom::Element const& el) {
    // - input elements to which the readonly attribute applies, and that are mutable (i.e. that do not have the
    //   readonly attribute specified and that are not disabled)
    if (_matchTextLikeInput(el) or _matchDateTimeInput(el))
        return not el.hasAttribute(Html::READONLY_ATTR) and not _matchDisabledFormControl(el);

    // - textarea elements that do not have a readonly attribute, and that are not disabled
    if (el.qualifiedName == Html::TEXTAREA_TAG)
        return not el.hasAttribute(Html::READONLY_ATTR) and not _matchDisabledFormControl(el);

    // - elements that are editing hosts or editable and are neither input elements nor textarea elements
    return el.qualifiedName != Html::INPUT_TAG and _matchEditable(el);
}

// MARK: Tree State ------------------------------------------------------------

// https://www.w3.org/TR/selectors-4/#the-empty-pseudo
static bool _matchEmpty(Dom::Element const& el) {
    // The :empty pseudo-class represents an element that has no children except, optionally, document white space
    // characters. In terms of the document tree, only element nodes and content nodes (such as DOM text nodes, and
    // entity references) whose data has a non-zero length must be considered as affecting emptiness; comments,
    // processing instructions, and other nodes must not affect whether an element is considered empty or not.
    for (auto child : el.iterChildren()) {
        if (child->is<Dom::Element>())
            return false;
        if (auto text = child->as<Dom::Text>())
            for (auto rune : iterRunes(text->data()))
                if (not isAsciiSpace(rune))
                    return false;
    }
    return true;
}

// MARK: Matching --------------------------------------------------------------

// NOTE: There is no scripting nor user interaction, so the checkedness, selectedness, and value of form controls
//       are given by their checked, selected, and value attributes, and indeterminateness is always false.
export bool matchPseudoClass(Dom::Element const& el, PseudoClass pseudoClass) {
    switch (pseudoClass) {
    // https://www.w3.org/TR/selectors-4/#the-any-link-pseudo
    // It matches an element if the element would match either :link or :visited, and is equivalent to
    // :is(:link, :visited).
    case PseudoClass::ANY_LINK:
    // https://html.spec.whatwg.org/multipage/semantics-other.html#selector-link
    // All a elements that have an href attribute, and all area elements that have an href attribute, must match one
    // of :link and :visited.
    case PseudoClass::LINK:
        return Html::isHyperlinkElement(el.qualifiedName) and el.hasAttribute(Html::HREF_ATTR);

    // https://www.w3.org/TR/selectors-4/#the-scope-pseudo
    // NOTE: Style sheets have no scoping root, so :scope represents the document element.
    case PseudoClass::SCOPE:
    // https://www.w3.org/TR/selectors-4/#root-pseudo
    case PseudoClass::ROOT:
        return el.isDocumentElement();

    case PseudoClass::EMPTY:
        return _matchEmpty(el);

    // https://html.spec.whatwg.org/multipage/semantics-other.html#selector-checked
    case PseudoClass::CHECKED:
        // The :checked pseudo-class must match any element falling into one of the following categories:
        // - input elements whose type attribute is in the Checkbox state and whose checkedness state is true
        // - input elements whose type attribute is in the Radio Button state and whose checkedness state is true
        // - option elements whose selectedness is true
        if (_matchInput(el, "checkbox"s, "radio"s))
            return el.hasAttribute(Html::CHECKED_ATTR);
        return el.qualifiedName == Html::OPTION_TAG and el.hasAttribute(Html::SELECTED_ATTR);

    // https://html.spec.whatwg.org/multipage/semantics-other.html#selector-default
    case PseudoClass::DEFAULT:
        // The :default pseudo-class must match any element falling into one of the following categories:
        // - Submit buttons that are default buttons of their form owner.
        // - input elements to which the checked attribute applies and that have a checked attribute
        // - option elements that have a selected attribute
        if (_matchInput(el, "checkbox"s, "radio"s))
            return el.hasAttribute(Html::CHECKED_ATTR);
        if (el.qualifiedName == Html::OPTION_TAG)
            return el.hasAttribute(Html::SELECTED_ATTR);
        return _matchDefaultButton(el);

    // https://html.spec.whatwg.org/multipage/semantics-other.html#selector-indeterminate
    case PseudoClass::INDETERMINATE:
        // The :indeterminate pseudo-class must match any element falling into one of the following categories:
        // - input elements whose type attribute is in the Radio Button state and whose radio button group contains no
        //   input elements whose checkedness state is true.
        // - progress elements with no value content attribute
        if (_matchInput(el, "radio"s))
            return not _matchRadioGroupChecked(el);
        return el.qualifiedName == Html::PROGRESS_TAG and not el.hasAttribute(Html::VALUE_ATTR);

    // https://html.spec.whatwg.org/multipage/semantics-other.html#selector-disabled
    case PseudoClass::DISABLED:
        // The :disabled pseudo-class must match any element that is actually disabled.
        return _matchActuallyDisabled(el);

    // https://html.spec.whatwg.org/multipage/semantics-other.html#selector-enabled
    case PseudoClass::ENABLED:
        // The :enabled pseudo-class must match any button, input, select, textarea, optgroup, option, fieldset element,
        // or form-associated custom element that is not actually disabled.
        return Html::isDisableableElement(el.qualifiedName) and not _matchActuallyDisabled(el);

    // https://html.spec.whatwg.org/multipage/semantics-other.html#selector-open
    case PseudoClass::OPEN:
        // The :open pseudo-class must match any element falling into one of the following categories:
        // - details elements that have an open attribute
        // - dialog elements that have an open attribute
        return Html::isOpenableElement(el.qualifiedName) and el.hasAttribute(Html::OPEN_ATTR);

    // https://html.spec.whatwg.org/multipage/semantics-other.html#selector-placeholder-shown
    case PseudoClass::PLACEHOLDER_SHOWN:
        // The :placeholder-shown pseudo-class must match any element falling into one of the following categories:
        // - input elements that have a placeholder attribute whose value is currently being presented to the user
        // - textarea elements that have a placeholder attribute whose value is currently being presented to the user
        if (not el.hasAttribute(Html::PLACEHOLDER_ATTR))
            return false;
        if (_matchTextLikeInput(el))
            return el.getAttribute(Html::VALUE_ATTR).unwrapOr(""s).len() == 0;
        return el.qualifiedName == Html::TEXTAREA_TAG and el.textContent().len() == 0;

    // https://html.spec.whatwg.org/multipage/semantics-other.html#selector-read-write
    case PseudoClass::READ_WRITE:
        // The :read-write pseudo-class must match any element falling into one of the following categories, which for
        // the purposes of Selectors are thus considered user-alterable:
        return _matchReadWrite(el);

    // https://html.spec.whatwg.org/multipage/semantics-other.html#selector-read-only
    case PseudoClass::READ_ONLY:
        // The :read-only pseudo-class must match all other HTML elements.
        return el.namespaceUri() == Html::NAMESPACE and not _matchReadWrite(el);

    // https://html.spec.whatwg.org/multipage/semantics-other.html#selector-required
    case PseudoClass::REQUIRED:
        // The :required pseudo-class must match any element falling into one of the following categories:
        // - input elements that are required
        // - select elements that have a required attribute
        // - textarea elements that have a required attribute
        if (el.qualifiedName == Html::INPUT_TAG)
            return _matchRequirableInput(el) and el.hasAttribute(Html::REQUIRED_ATTR);
        return Html::isRequirableElement(el.qualifiedName) and el.hasAttribute(Html::REQUIRED_ATTR);

    // https://html.spec.whatwg.org/multipage/semantics-other.html#selector-optional
    case PseudoClass::OPTIONAL:
        // The :optional pseudo-class must match any element falling into one of the following categories:
        // - input elements to which the required attribute applies that are not required
        // - select elements that do not have a required attribute
        // - textarea elements that do not have a required attribute
        if (el.qualifiedName == Html::INPUT_TAG)
            return _matchRequirableInput(el) and not el.hasAttribute(Html::REQUIRED_ATTR);
        return Html::isRequirableElement(el.qualifiedName) and not el.hasAttribute(Html::REQUIRED_ATTR);

    default:
        return false;
    }
}

} // namespace Vaev::Style
