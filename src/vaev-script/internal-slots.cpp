export module Vaev.Script:internalSlots;

import Karm.Core;

import :agent;
import :completion;
import :properties;
import :value;

using namespace Karm;

namespace Vaev::Script {

// https://tc39.es/ecma262/#sec-object-type
export struct Object;

// MARK: Ordinary Object Internal Methods and Internal Slots -------------------
// https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots

// https://tc39.es/ecma262/#sec-ordinarygetprototypeof
Except<Gc::Ptr<Object>> ordinaryGetPrototypeOf(Object& self);

// https://tc39.es/ecma262/#sec-ordinaryisextensible
Except<Boolean> ordinaryIsExtensible(Object& self);

// https://tc39.es/ecma262/#sec-ordinarygetownproperty
Except<Opt<PropertyDescriptor>> ordinaryGetOwnProperty(Object& self, PropertyKey key);

// https://tc39.es/ecma262/#sec-ordinarydefineownproperty
export Except<Boolean> ordinaryDefineOwnProperty(Object& self, PropertyKey key, PropertyDescriptor desc);

// https://tc39.es/ecma262/#sec-ordinaryget
export Except<Value> ordinaryGet(Object& self, PropertyKey key, Value receiver);

// https://tc39.es/ecma262/#sec-ordinaryset
export Except<Boolean> ordinarySet(Object& self, PropertyKey key, Value value, Value receiver);

export struct InternalMethods {
    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-getprototypeof
    Except<Gc::Ptr<Object>> (*getPrototypeOf)(Object& self) = ordinaryGetPrototypeOf;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-setprototypeof-v
    Except<Boolean> (*setPrototypeOf)(Object& self, Gc::Ptr<Object> v) = nullptr;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-isextensible
    Except<Boolean> (*isExtensible)(Object& self) = ordinaryIsExtensible;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-preventextensions
    Except<Boolean> (*preventExtensions)(Object& self) = nullptr;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-getownproperty-p
    Except<Opt<PropertyDescriptor>> (*getOwnProperty)(Object& self, PropertyKey key) = ordinaryGetOwnProperty;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-defineownproperty-p-desc
    Except<Boolean> (*defineOwnProperty)(Object& self, PropertyKey key, PropertyDescriptor desc) = ordinaryDefineOwnProperty;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-hasproperty-p
    Except<Boolean> (*hasProperty)(Object& self, PropertyKey key) = nullptr;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-get-p-receiver
    Except<Value> (*get)(Object& self, PropertyKey key, Value receiver) = ordinaryGet;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-set-p-v-receiver
    Except<Boolean> (*set)(Object& self, PropertyKey key, Value v, Value receiver) = ordinarySet;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-delete-p
    Except<Boolean> (*delete_)(Object& self, PropertyKey key) = nullptr;

    // https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots-ownpropertykeys
    Except<Vec<PropertyKey>> (*ownPropertyKeys)(Object& self) = nullptr;

    // Additional Essential Internal Methods of Function Objects
    // https://tc39.es/ecma262/#table-additional-essential-internal-methods-of-function-objects

    Except<Value> (*call)(Object& self, Gc::Ref<Object> thisArg, Slice<Value> args) = nullptr;

    Except<Value> (*construct)(Object& self, Slice<Value> args, Gc::Ref<Object> newTarget) = nullptr;
};

} // namespace Vaev::Script
