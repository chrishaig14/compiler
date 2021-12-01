//
// Created by chris on 21/11/21.
//

#ifndef XLANG_OBJECTMETHODFROMINSTANCE_H
#define XLANG_OBJECTMETHODFROMINSTANCE_H

#include "Exp.h"
#include "InstanceObject.h"

class sem::ObjectMethodFromInstance : public sem::Exp {
public:
    UExp object;
    InstanceObject instance;
    std::string method;

    ObjectMethodFromInstance(UExp object, const InstanceObject& instance, std::string method);

    ObjectMethodFromInstance(const ObjectMethodFromInstance& other)
            : sem::Exp(ExpType::METHOD_FROM_INSTANCE), instance(other.instance) {
        this->object = other.object->clone();
        this->method = other.method;
    }

    bool equals(const Exp& o) const override;
    UExp clone() const override;

};


#endif //XLANG_OBJECTMETHODFROMINSTANCE_H
