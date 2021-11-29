//
// Created by chris on 21/11/21.
//

#include "StaticMethodFromInstance.h"

bool sem::StaticMethodFromInstance::equals(const sem::Exp& o) const {
    return false;
}

sem::StaticMethodFromInstance::StaticMethodFromInstance(std::unique_ptr<InstanceObject> instance, std::string method)
        : Exp(ExpType::STATIC_METHOD_FROM_INSTANCE), instance(std::move(instance)), method(method) {
}
