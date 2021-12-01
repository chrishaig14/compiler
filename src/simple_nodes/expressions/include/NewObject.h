//
// Created by chris on 11/4/21.
//

#ifndef XLANG_NEWOBJECT_H
#define XLANG_NEWOBJECT_H


#include <string>
#include <vector>
#include "Exp.h"

class sem::NewObject : public sem::Exp {
public:
    std::string class_name;
    std::vector<UExp> args;

    NewObject();

    NewObject(const NewObject& other) : sem::Exp(ExpType::NEW) {
        this->class_name = other.class_name;
        for (auto& a: other.args) {
            this->args.emplace_back(a->clone());
        }
    }

    bool equals(const Exp& o) const override;
    UExp clone() const override;
};


#endif //XLANG_NEWOBJECT_H
