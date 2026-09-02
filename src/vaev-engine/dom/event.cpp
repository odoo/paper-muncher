export module Vaev.Engine:dom.event;

import Karm.Core;
import :dom.window;

using namespace Karm;

namespace Vaev::Dom {

using _EventTarget = Union<
    Gc::Ref<Window>,
    Gc::Ref<Node>,
    Gc::Ref<PseudoElement>>;

export struct EventTarget : _EventTarget {
    using Union::Union;

    static EventTarget fromOriginatingElement(OriginatingElement el) {
        return el.visit(
            [](Gc::Ref<Element> element) {
                return EventTarget{Gc::Ref<Node>{element}};
            },
            [](Gc::Ref<PseudoElement> pseudoElement) {
                return EventTarget{pseudoElement};
            }
        );
    }
};

export enum struct EventType {
    CONTEXTMENU, //< https://w3c.github.io/pointerevents/#contextmenu

    _LEN,
};

export enum struct EventFlags {
    /// https://dom.spec.whatwg.org/#stop-propagation-flag
    STOP_PROPAGATION = 1 << 0,

    // https://dom.spec.whatwg.org/#stop-immediate-propagation-flag
    STOP_IMMEDIATE_PROPAGATION = 1 << 1,

    // https://dom.spec.whatwg.org/#canceled-flag
    CANCELED = 1 << 2,

    // https://dom.spec.whatwg.org/#in-passive-listener-flag
    IN_PASSIVE_LISTENER = 1 << 3,

    // https://dom.spec.whatwg.org/#composed-flag
    COMPOSED = 1 << 4,

    // https://dom.spec.whatwg.org/#initialized-flag
    INITIALIZED = 1 << 5,

    // https://dom.spec.whatwg.org/#dispatch-flag
    DISPATCH = 1 << 6
};

// https://dom.spec.whatwg.org/#interface-event
export struct Event {
    EventType type;
    Flags<EventFlags> flags;
    Opt<EventTarget> target;

    virtual ~Event() = default;

    virtual bool is(Meta::Id id) const {
        return id == Meta::idOf<Event>();
    }

    template <Meta::Derive<Event> T>
    Opt<T&> as() {
        if (is(Meta::idOf<T>()))
            return Some(static_cast<T&>(*this));
        return NONE;
    }

    template <Meta::Derive<Event> T>
    Opt<T const&> as() const {
        if (is(Meta::idOf<T>()))
            return Some(static_cast<T const&>(*this));
        return NONE;
    }

    void stopPropagation() {
        flags.set(EventFlags::STOP_PROPAGATION);
    }
};

// https://www.w3.org/TR/uievents/#idl-uievent
export struct UiEvent : Event {
    bool is(Meta::Id id) const override {
        return id == Meta::idOf<UiEvent>() or Event::is(id);
    }
};

// https://www.w3.org/TR/pointerevents/#dom-mouseevent
export struct MouseEvent : UiEvent {
    Math::Vec2i screen;

    bool is(Meta::Id id) const override {
        return id == Meta::idOf<MouseEvent>() or UiEvent::is(id);
    }
};

} // namespace Vaev::Dom
