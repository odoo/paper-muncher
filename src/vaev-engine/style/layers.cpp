export module Vaev.Engine:style.layers;

import Karm.Core;

import :style.origin;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-cascade-5/#typedef-layer-name
export using LayerName = Vec<Symbol>;

export using Layer = Distinct<usize, struct _LayerTag>;

// https://drafts.csswg.org/css-cascade-5/#layer-ordering
export struct CascadeLayers {
    struct Node {
        Opt<Symbol> name = NONE;
        Vec<Layer> children = {};
        usize rank = 0;
    };

    Vec<Node> _nodes;

    CascadeLayers() {
        for (usize i = 0; i < toUnderlyingType(Origin::_LEN); i++)
            _nodes.pushBack(Node{});
    }

    Layer root(Origin origin) const {
        return Layer{toUnderlyingType(origin)};
    }

    Node& _node(Layer layer) {
        return _nodes[layer.value()];
    }

    Node const& _node(Layer layer) const {
        return _nodes[layer.value()];
    }

    Layer _append(Layer parent, Opt<Symbol> name) {
        Layer layer{_nodes.len()};
        _nodes.pushBack(Node{.name = name});
        _node(parent).children.pushBack(layer);
        return layer;
    }

    Opt<Layer> _lookup(Layer parent, Symbol name) const {
        for (auto child : _node(parent).children)
            if (_node(child).name == name)
                return Some(child);
        return NONE;
    }

    // https://drafts.csswg.org/css-cascade-5/#layer-names
    Layer declare(Layer parent, LayerName const& name) {
        for (auto const& segment : name)
            parent = _lookup(parent, segment).unwrapOrElse([&] {
                return _append(parent, Some(segment));
            });
        return parent;
    }

    // https://drafts.csswg.org/css-cascade-5/#unnamed-layer
    Layer declareAnonymous(Layer parent) {
        return _append(parent, NONE);
    }

    void _assignRanks(Layer layer, usize& rank) {
        for (auto child : _node(layer).children)
            _assignRanks(child, rank);
        _node(layer).rank = rank++;
    }

    void finalize() {
        usize rank = 0;
        for (usize i = 0; i < toUnderlyingType(Origin::_LEN); i++)
            _assignRanks(root(Origin{static_cast<u8>(i)}), rank);
    }

    usize rank(Layer layer) const {
        return _node(layer).rank;
    }
};

} // namespace Vaev::Style
