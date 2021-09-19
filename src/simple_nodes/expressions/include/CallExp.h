//
// Created by chris on 4/4/21.
//

#ifndef XLANG_CALLEXP_H
#define XLANG_CALLEXP_H


#include <vector>
#include "Exp.h"

class sem::CallExp : public sem::Exp {
public:
    UExp function;
    std::vector<UExp> arguments;
    CallExp(UExp function, std::vector<UExp> arguments);

    bool equals(const Exp& o) const override;
    ~CallExp() = default;
};


#endif //XLANG_CALLEXP_H
