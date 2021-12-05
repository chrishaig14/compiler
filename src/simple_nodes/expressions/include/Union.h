//
// Created by chris on 12/4/21.
//

#ifndef XLANG_UNION_H
#define XLANG_UNION_H


#include <vector>
#include "Exp.h"
#include <cassert>

class sem::Union : public sem::Exp {

public:
    UExp exp;
    int type_index;

    explicit Union(UExp exp, int type_index) : sem::Exp(ExpType::UNION), exp(std::move(exp)) {
        this->type_index = type_index;
    }

    Union(const Union& other) : sem::Exp(ExpType::UNION) {
        this->exp = other.exp->clone();
        this->type_index = other.type_index;
    }

    bool equals(const Exp& o) const override;
    UExp clone() const override;
};


#endif //XLANG_UNION_H
