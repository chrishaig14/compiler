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
    std::unique_ptr<InstanceObject> instance;
    std::string method;

    ObjectMethodFromInstance(UExp object, std::unique_ptr<InstanceObject> instance, std::string method);

    bool equals(const Exp& o) const override;

};


#endif //XLANG_OBJECTMETHODFROMINSTANCE_H
