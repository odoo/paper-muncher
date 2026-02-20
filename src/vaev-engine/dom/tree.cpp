export module Vaev.Engine:dom.tree;

import Karm.Core;
import Karm.Gc;

using namespace Karm;

namespace Vaev::Dom {

export template <typename Node>
struct Tree : Meta::Pinned {
    Gc::Ptr<Node> _parent = nullptr;
    Gc::Ptr<Node> _firstChild = nullptr;
    Gc::Ptr<Node> _lastChild = nullptr;
    Gc::Ptr<Node> _nextSibling = nullptr;
    Gc::Ptr<Node> _prevSibling = nullptr;

    // Accessor ----------------------------------------------------------------

    usize index(auto filter) const {
        usize index = 0;
        for (auto node = previousSibling(); node; node = node->previousSibling())
            if (filter(node))
                ++index;
        return index;
    }

    usize index() const {
        return index([](auto) {
            return true;
        });
    }

    usize reverseIndex(auto filter) const {
        usize index = 0;
        for (auto node = nextSibling(); node; node = node->nextSibling())
            if (filter(node))
                ++index;
        return index;
    }

    bool hasParentNode() const { return _parent != nullptr; }

    Gc::Ptr<Node> parentNode() const { return _parent; }

    bool hasChildren() const { return _firstChild != nullptr; }

    usize countChildren() const {
        usize count = 0;
        for (auto child = firstChild(); child; child = child->nextSibling())
            ++count;
        return count;
    }

    Gc::Ptr<Node> firstChild() const { return _firstChild; }

    Gc::Ptr<Node> lastChild() const { return _lastChild; }

    bool hasPreviousSibling() const { return _prevSibling != nullptr; }

    Gc::Ptr<Node> previousSibling() const { return _prevSibling; }

    bool hasNextSibling() const { return _nextSibling != nullptr; }

    Gc::Ptr<Node> nextSibling() const { return _nextSibling; }

    // Insertion & Deletion ----------------------------------------------------

    void appendChild(Gc::Ptr<Node> node) {
        if (node->_parent)
            node->remove();

        if (_lastChild)
            _lastChild->_nextSibling = node;
        node->_prevSibling = _lastChild;
        node->_parent = {MOVE, static_cast<Node*>(this)};
        _lastChild = node;
        if (!_firstChild)
            _firstChild = _lastChild;
    }

    void prependChild(Gc::Ptr<Node> node) {
        if (node->_parent)
            node->remove();

        if (_firstChild)
            _firstChild->_prevSibling = node;
        node->_nextSibling = _firstChild;
        node->_parent = static_cast<Node*>(this);
        _firstChild = node;
        if (!_lastChild)
            _lastChild = _firstChild;
    }

    void insertBefore(Gc::Ptr<Node> node, Gc::Ptr<Node> child) {
        if (!child)
            return appendChild(node);

        if (node->_parent)
            node->remove();

        node->_prevSibling = child->_prevSibling;
        node->_nextSibling = child;

        if (child->_prevSibling)
            child->_prevSibling->_nextSibling = node;

        if (_firstChild == child)
            _firstChild = node;

        child->_prevSibling = node;

        node->_parent = {MOVE, static_cast<Node*>(this)};
    }

    void insertAfter(Gc::Ptr<Node> node, Gc::Ptr<Node> child) {
        if (not child)
            return appendChild(node);

        if (node->_parent)
            node->remove();

        node->_prevSibling = child;
        node->_nextSibling = child->_nextSibling;

        if (child->_nextSibling)
            child->_nextSibling->_prevSibling = node;

        if (_lastChild == child)
            _lastChild = node;

        child->_nextSibling = node;

        node->_parent = {MOVE, static_cast<Node*>(this)};
    }

    void remove(this auto& self) {
        if (not self._parent)
            return;

        if (self._parent->_firstChild == &self)
            self._parent->_firstChild = self._nextSibling;

        if (self._parent->_lastChild == &self)
            self._parent->_lastChild = self._prevSibling;

        if (self._nextSibling)
            self._nextSibling->_prevSibling = self._prevSibling;

        if (self._prevSibling)
            self._prevSibling->_nextSibling = self._nextSibling;

        self._nextSibling = nullptr;
        self._prevSibling = nullptr;
        self._parent = nullptr;
    }


    void removeChild(this auto& self, Gc::Ptr<Node> node) {
        if (node->_parent != &self)
            panic("node is not a child");

        node->remove();
    }

    // Iteration ---------------------------------------------------------------

    Yield<Gc::Ref<Node>> iterDepthFirst(this auto& self) {
        co_yield Gc::Ref(self);
        for (auto child = self.firstChild(); child; child = child->nextSibling())
            for (auto i : child->iterDepthFirst())
                co_yield i;
    }

    Yield<Gc::Ref<Node>> iterChildren() {
        for (auto child = firstChild(); child; child = child->nextSibling())
            co_yield child.upgrade();
    }
};

} // namespace Vaev::Dom
