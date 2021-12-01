//
// Created by chris on 21/11/21.
//

#ifndef XLANG_STATICMETHODFROMINSTANCE_H
#define XLANG_STATICMETHODFROMINSTANCE_H

#include "Exp.h"
#include "InstanceObject.h"

class sem::StaticMethodFromInstance : public sem::Exp {
public:
    InstanceObject instance;
    std::string method;

    StaticMethodFromInstance(const InstanceObject& instance, std::string method);

    bool equals(const Exp& o) const override;
    UExp clone() const override;

};


#endif //XLANG_STATICMETHODFROMINSTANCE_H
