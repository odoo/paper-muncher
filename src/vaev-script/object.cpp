export module Vaev.Script:object;

import Karm.Core;

import :agent;
import :completion;
import :properties;
import :value;
import :internalSlots;

using namespace Karm;

namespace Vaev::Script {

// https://tc39.es/ecma262/#sec-object-type
export struct Object;

// MARK: The Object Type -------------------------------------------------------
// https://tc39.es/ecma262/#sec-object-type

export struct _ObjectCreateArgs {
    Gc::Ptr<Object> prototype = nullptr;
};

export struct Object {
    Agent& agent;
    InternalMethods internalMethods = {};
    PropertyStorage propertyStorage = {};
    Gc::Ptr<Object> prototype = nullptr;
    bool extensible = true;

    static Gc::Ref<Object> create(Agent& agent, _ObjectCreateArgs args = {});

    Except<Gc::Ptr<Object>> getPrototypeOf();

    Except<Boolean> setPrototypeOf(Gc::Ptr<Object> v);

    Except<Boolean> isExtensible();

    Except<Boolean> preventExtensions();

    Except<Opt<PropertyDescriptor>> getOwnProperty(PropertyKey key);

    Except<Boolean> defineOwnProperty(PropertyKey key, PropertyDescriptor desc);

    Except<Boolean> hasProperty(PropertyKey key);

    Except<Value> get(PropertyKey key, Value receiver);

    Except<Boolean> set(PropertyKey key, Value value, Value receiver);

    Except<Boolean> delete_(PropertyKey key);

    Except<Vec<PropertyKey>> ownPropertyKeys();

    Except<Value> call(Gc::Ref<Object> thisArg, Slice<Value> args);

    Except<Value> construct(Slice<Value> args, Gc::Ref<Object> newTarget);

    void repr(Io::Emit& e) const;

    Gc::Ref<Object> ref();
};

} // namespace Vaev::Script
