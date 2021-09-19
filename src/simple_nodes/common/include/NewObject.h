//
// Created by chris on 11/4/21.
//

#ifndef XLANG_NEWOBJECT_H
#define XLANG_NEWOBJECT_H


#include <string>
#include <vector>
#include "../../expressions/include/Exp.h"

class sem::NewObject : public sem::Exp {
public:
    std::string class_name;
    std::vector<UExp> args;

    NewObject();

    bool equals(const Exp& o) const override;
};


#endif //XLANG_NEWOBJECT_H
