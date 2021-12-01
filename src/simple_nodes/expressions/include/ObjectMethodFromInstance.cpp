//
// Created by chris on 21/11/21.
//

#include "ObjectMethodFromInstance.h"

bool sem::ObjectMethodFromInstance::equals(const sem::Exp& o) const {
    return false;
}

sem::ObjectMethodFromInstance::ObjectMethodFromInstance(sem::UExp object, const InstanceObject& instance,
                                                        std::string method)
        : Exp(ExpType::METHOD_FROM_INSTANCE), object(std::move(object)), instance(instance), method(method) {
}

sem::UExp sem::ObjectMethodFromInstance::clone() const {
    return std::make_unique<ObjectMethodFromInstance>(*this);
}
