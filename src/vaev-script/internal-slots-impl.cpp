module;

#include <karm/macros>

module Vaev.Script;

import Karm.Core;
import Karm.Gc;

namespace Vaev::Script {

// MARK: Ordinary Object Ordinary Methods --------------------------------------
// https://tc39.es/ecma262/#sec-ordinary-object-internal-methods-and-internal-slots

// https://tc39.es/ecma262/#sec-ordinarygetprototypeof
Except<Gc::Ptr<Object>> ordinaryGetPrototypeOf(Object& self) {
    return Ok(self.prototype);
}

// https://tc39.es/ecma262/#sec-ordinaryisextensible
Except<Boolean> ordinaryIsExtensible(Object& self) {
    return Ok(self.extensible);
}

// https://tc39.es/ecma262/#sec-ordinarygetownproperty
Except<Opt<PropertyDescriptor>> ordinaryGetOwnProperty(Object& self, PropertyKey key) {
    // 1. If obj does not have an own property with key propertyKey, return undefined.
    if (not self.propertyStorage.has(key))
        return Ok(NONE);
    // 2. Let desc be a newly created Property Descriptor with no fields.
    auto desc = PropertyDescriptor{};
    // 3. Let ownProperty be obj's own property whose key is propertyKey.
    auto ownProperty = self.propertyStorage.get(key).unwrap();
    // 4. If ownProperty is a data property, then
    if (ownProperty.isData()) {
        //    a. Set desc.[[Value]] to the value of ownProperty's [[Value]] attribute.
        desc.value = ownProperty.value.unwrap<Value>();
        //    b. Set desc.[[Writable]] to the value of ownProperty's [[Writable]] attribute.
        desc.writable = ownProperty.attributes.writable;
    }
    // 5. Else,
    else {
        //    a. Assert: ownProperty is an accessor property.
        auto accessor = ownProperty.value.unwrap<PropertyStorage::Accessor>();
        //    b. Set desc.[[Get]] to the value of ownProperty's [[Get]] attribute.
        desc.get = accessor.get;
        //    c. Set desc.[[Set]] to the value of ownProperty's [[Set]] attribute.
        desc.set = accessor.set;
    }
    // 6. Set desc.[[Enumerable]] to the value of ownProperty's [[Enumerable]] attribute.
    desc.enumerable = ownProperty.attributes.enumerable;
    // 7. Set desc.[[Configurable]] to the value of ownProperty's [[Configurable]] attribute.
    desc.configurable = ownProperty.attributes.configurable;

    // 8. Return desc.
    return Ok(desc);
}

// https://tc39.es/ecma262/#sec-validateandapplypropertydescriptor
static Except<Boolean> _validateAndApplyPropertyDescriptor(Gc::Ptr<Object> self, PropertyKey key, bool extensible, PropertyDescriptor desc, Opt<PropertyDescriptor> current) {
    // 1. Assert: P is a property key.
    // 2. If current is undefined, then
    if (current == NONE) {
        //    a. If extensible is false, return false.
        if (not extensible)
            return Ok(false);

        //    b. If O is undefined, return true.
        if (not self)
            return Ok(true);

        auto& object = *self;

        //    c. If IsAccessorDescriptor(Desc) is true, then
        if (desc.isAccessorDescriptor()) {
            // i. Create an own accessor property named P of object O whose
            //    [[Get]], [[Set]], [[Enumerable]], and [[Configurable]]
            //    attributes are set to the value of the corresponding field
            //    in Desc if Desc has that field, or to the attribute's
            //    default value otherwise.
            object.propertyStorage.set(
                key,
                {
                    .value = PropertyStorage::Accessor{
                        .get = desc.get.unwrapOr(nullptr),
                        .set = desc.set.unwrapOr(nullptr),
                    },
                    .attributes = {
                        .writable = false,
                        .enumerable = desc.enumerable.unwrapOr(false),
                        .configurable = desc.configurable.unwrapOr(false),
                    },
                }
            );

            // d. Else,
        } else {
            // i. Create an own data property named P of object O whose
            //    [[Value]], [[Writable]], [[Enumerable]], and [[Configurable]]
            //    attributes are set to the value of the corresponding field in
            //    Desc if Desc has that field, or to the attribute's default
            //    value otherwise.
            object.propertyStorage.set(
                key,
                {
                    .value = desc.value,
                    .attributes = {
                        .writable = desc.writable.unwrapOr(false),
                        .enumerable = desc.enumerable.unwrapOr(false),
                        .configurable = desc.configurable.unwrapOr(false),
                    },
                }
            );
        }
        //    e. Return true.
        return Ok(true);
    }

    // 3. Assert: current is a fully populated Property Descriptor.

    // 4. If Desc does not have any fields, return true.
    if (desc.empty())
        return Ok(true);

    // 5. If current.[[Configurable]] is false, then
    if (not current->configurable) {
        //    a. If Desc has a [[Configurable]] field and Desc.[[Configurable]] is true, return false.
        if (desc.configurable and desc.configurable == true)
            return Ok(false);

        //    b. If Desc has an [[Enumerable]] field and Desc.[[Enumerable]] is not current.[[Enumerable]], return false.
        if (desc.enumerable and desc.enumerable != current->enumerable)
            return Ok(false);

        //    c. If IsGenericDescriptor(Desc) is false and IsAccessorDescriptor(Desc) is not IsAccessorDescriptor(current), return false.
        if (not desc.isGenericDescriptor() and desc.isAccessorDescriptor() != current->isAccessorDescriptor())
            return Ok(false);

        //    d. If IsAccessorDescriptor(current) is true, then
        if (current->isGenericDescriptor()) {

            //       i. If Desc has a [[Get]] field and SameValue(Desc.[[Get]], current.[[Get]]) is false, return false.
            //       ii. If Desc has a [[Set]] field and SameValue(Desc.[[Set]], current.[[Set]]) is false, return false.

            //    e. Else if current.[[Writable]] is false, then
        } else if (current->writable == false) {
            //       i. If Desc has a [[Writable]] field and Desc.[[Writable]] is true, return false.
            if (desc.writable != NONE and desc.writable == true)
                return Ok(false);

            //       ii. NOTE: SameValue returns true for NaN values which may be distinguishable by other means. Returning here ensures that any existing property of O remains unmodified.
            //       iii. If Desc has a [[Value]] field, return SameValue(Desc.[[Value]], current.[[Value]]).
        }
    }

    // 6. If O is not undefined, then
    if (self) {

        //    a. If IsDataDescriptor(current) is true and IsAccessorDescriptor(Desc) is true, then
        if (current->isDataDescriptor() and desc.isAccessorDescriptor()) {
            //       i. If Desc has a [[Configurable]] field, let configurable be Desc.[[Configurable]]; else let configurable be current.[[Configurable]].
            //       ii. If Desc has a [[Enumerable]] field, let enumerable be Desc.[[Enumerable]]; else let enumerable be current.[[Enumerable]].
            //       iii. Replace the property named P of object O with an accessor property whose [[Configurable]] and [[Enumerable]] attributes are set to configurable and enumerable, respectively, and whose [[Get]] and [[Set]] attributes are set to the value of the corresponding field in Desc if Desc has that field, or to the attribute's default value otherwise.

            //    b. Else if IsAccessorDescriptor(current) is true and IsDataDescriptor(Desc) is true, then
        } else if (current->isAccessorDescriptor() and desc.isDataDescriptor()) {
            //       i. If Desc has a [[Configurable]] field, let configurable be Desc.[[Configurable]]; else let configurable be current.[[Configurable]].
            //       ii. If Desc has a [[Enumerable]] field, let enumerable be Desc.[[Enumerable]]; else let enumerable be current.[[Enumerable]].
            //       iii. Replace the property named P of object O with a data property whose [[Configurable]] and [[Enumerable]] attributes are set to configurable and enumerable, respectively, and whose [[Value]] and [[Writable]] attributes are set to the value of the corresponding field in Desc if Desc has that field, or to the attribute's default value otherwise.

            //    c. Else,
        } else {
            //       i. For each field of Desc, set the corresponding attribute of the property named P of object O to the value of the field.
        }
    }
    // 7. Return true.
    return Ok(true);
}

// https://tc39.es/ecma262/#sec-ordinarydefineownproperty
Except<Boolean> ordinaryDefineOwnProperty(Object& self, PropertyKey key, PropertyDescriptor desc) {
    // 1. Let current be ? O.[[GetOwnProperty]](P).
    auto current = try$(self.getOwnProperty(key));

    // 2. Let extensible be ? IsExtensible(O).
    auto extensible = try$(self.isExtensible());

    // 3. Return ValidateAndApplyPropertyDescriptor(O, P, extensible, Desc, current).
    return _validateAndApplyPropertyDescriptor(self, key, extensible, desc, current);
}

// https://tc39.es/ecma262/#sec-ordinaryget
Except<Value> ordinaryGet(Object& self, PropertyKey key, Value receiver) {
    // 1. Let desc be ? O.[[GetOwnProperty]](P).
    auto maybeDesc = try$(self.getOwnProperty(key));

    // 2. If desc is undefined, then
    if (maybeDesc == NONE) {
        //    a. Let parent be ? O.[[GetPrototypeOf]]().
        auto parent = try$(self.getPrototypeOf());

        //    b. If parent is null, return undefined.
        if (parent == NONE)
            return Ok(undefined);

        //    c. Return ? parent.[[Get]](P, Receiver).
        return parent->get(key, receiver);
    }

    auto& desc = maybeDesc.unwrap();

    // 3. If IsDataDescriptor(desc) is true, return desc.[[Value]].
    if (desc.isDataDescriptor())
        return Ok(desc.value);

    // 4. Assert: IsAccessorDescriptor(desc) is true.
    if (not desc.isAccessorDescriptor())
        panic("expected accessor descriptor");

    // 5. Let getter be desc.[[Get]].
    auto getter = desc.get;

    // 6. If getter is undefined, return undefined.
    if (getter == NONE)
        return Ok(undefined);

    // 7. Return ? Call(getter, Receiver).
    return call(self.agent, *getter, receiver);
}

// https://tc39.es/ecma262/#sec-ordinarysetwithowndescriptor
Except<Boolean> ordinarySetWithOwnDescriptor(Object& self, PropertyKey key, Value value, Value receiver, Opt<PropertyDescriptor> ownDesc) {
    // 1. If ownDesc is undefined, then
    if (ownDesc == NONE) {
        //    a. Let parent be ? obj.[[GetPrototypeOf]]().
        auto parent = try$(self.getPrototypeOf());
        //    b. If parent is not null, return ? parent.[[Set]](propertyKey, value, receiver).
        if (parent != nullptr)
            return parent->set(key, value, receiver);
        //    c. Set ownDesc to the PropertyDescriptor { [[Value]]: undefined, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true }.
        ownDesc = PropertyDescriptor{
            .value = undefined,
            .writable = true,
            .enumerable = true,
            .configurable = true,
        };
    }

    // 2. If IsDataDescriptor(ownDesc) is true, then
    if (ownDesc->isDataDescriptor()) {
        //    a. If ownDesc.[[Writable]] is false, return false.
        if (ownDesc->writable == false)
            return Ok(false);

        //    b. If receiver is not an Object, return false.
        if (not receiver.isObject())
            return Ok(false);

        //    c. Let existingDescriptor be ? receiver.[[GetOwnProperty]](propertyKey).
        auto receiverObject = receiver.asObject();
        auto existingDescriptor = try$(receiverObject->getOwnProperty(key));

        //    d. If existingDescriptor is undefined, then
        if (existingDescriptor == NONE) {
            //       i. Assert: receiver does not currently have a property propertyKey.
            //       ii. Return ? CreateDataProperty(receiver, propertyKey, value).
            return createDataProperty(self, key, value);
        }

        //    e. If IsAccessorDescriptor(existingDescriptor) is true, return false.
        if (existingDescriptor->isAccessorDescriptor())
            return Ok(false);

        //    f. If existingDescriptor.[[Writable]] is false, return false.
        if (existingDescriptor->writable == false)
            return Ok(false);

        //    g. Let valueDesc be the PropertyDescriptor { [[Value]]: value }.
        auto valueDesc = PropertyDescriptor{
            .value = value,
        };

        //    h. Return ? receiver.[[DefineOwnProperty]](propertyKey, valueDesc).
        return receiverObject->defineOwnProperty(key, valueDesc);
    }

    // 3. Assert: IsAccessorDescriptor(ownDesc) is true.

    // 4. Let setter be ownDesc.[[Set]].
    auto setter = ownDesc->set;

    // 5. If setter is undefined, return false.
    if (setter == NONE)
        return Ok(false);

    // 6. Perform ? Call(setter, receiver, « value »).
    try$(call(self.agent, receiver, value));

    // 7. Return true.
    return Ok(true);
}

// https://tc39.es/ecma262/#sec-ordinaryset
Except<Boolean> ordinarySet(Object& self, PropertyKey key, Value value, Value receiver) {
    auto ownDesc = try$(self.getOwnProperty(key));
    return ordinarySetWithOwnDescriptor(self, key, value, receiver, ownDesc);
}

} // namespace Vaev::Script
