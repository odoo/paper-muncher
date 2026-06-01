module;

#include <karm/macros>

module Vaev.Script;

import Karm.Core;
import Karm.Gc;

namespace Vaev::Script {

// MARK: The Object Type -------------------------------------------------------
// https://tc39.es/ecma262/#sec-object-type

Gc::Ref<Object> Object::create(Agent& agent, _ObjectCreateArgs args) {
    auto obj = agent.heap.alloc<Object>(agent);
    obj->prototype = args.prototype;
    return obj;
}

Except<Gc::Ptr<Object>> Object::getPrototypeOf() {
    if (not internalMethods.getPrototypeOf)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.getPrototypeOf(*this);
}

Except<Boolean> Object::setPrototypeOf(Gc::Ptr<Object> v) {
    if (not internalMethods.setPrototypeOf)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.setPrototypeOf(*this, v);
}

Except<Boolean> Object::isExtensible() {
    if (not internalMethods.isExtensible)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.isExtensible(*this);
}

Except<Boolean> Object::preventExtensions() {
    if (not internalMethods.preventExtensions)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.preventExtensions(*this);
}

Except<Opt<PropertyDescriptor>> Object::getOwnProperty(PropertyKey key) {
    if (not internalMethods.getOwnProperty)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.getOwnProperty(*this, key);
}

Except<Boolean> Object::defineOwnProperty(PropertyKey key, PropertyDescriptor desc) {
    if (not internalMethods.defineOwnProperty)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.defineOwnProperty(*this, key, desc);
}

Except<Boolean> Object::hasProperty(PropertyKey key) {
    if (not internalMethods.hasProperty)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.hasProperty(*this, key);
}

Except<Value> Object::get(PropertyKey key, Value receiver) {
    if (not internalMethods.get)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.get(*this, key, receiver);
}

Except<Boolean> Object::set(PropertyKey key, Value value, Value receiver) {
    if (not internalMethods.set)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.set(*this, key, value, receiver);
}

Except<Boolean> Object::delete_(PropertyKey key) {
    if (not internalMethods.delete_)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.delete_(*this, key);
}

Except<Vec<PropertyKey>> Object::ownPropertyKeys() {
    if (not internalMethods.ownPropertyKeys)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.ownPropertyKeys(*this);
}

Except<Value> Object::call(Gc::Ref<Object> thisArg, Slice<Value> args) {
    if (not internalMethods.call)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.call(*this, thisArg, args);
}

Except<Value> Object::construct(Slice<Value> args, Gc::Ref<Object> newTarget) {
    if (not internalMethods.construct)
        return throwException(createException(agent, ExceptionType::TYPE_ERROR));
    return internalMethods.construct(*this, args, newTarget);
}

void Object::repr(Io::Emit& e) const {
    e("[object Object]");
}

Gc::Ref<Object> Object::ref() {
    return *this;
}

} // namespace Vaev::Script
