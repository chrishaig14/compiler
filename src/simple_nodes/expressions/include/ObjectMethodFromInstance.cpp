//
// Created by chris on 21/11/21.
//

#include "ObjectMethodFromInstance.h"

bool sem::ObjectMethodFromInstance::equals(const sem::Exp& o) const {
    return false;
}

sem::ObjectMethodFromInstance::ObjectMethodFromInstance(sem::UExp object, std::unique_ptr<InstanceObject> instance,
                                                        std::string method) : Exp(ExpType::METHOD_FROM_INSTANCE), object(std::move(object)), instance(std::move(instance)),
                                                                              method(method) {
}
