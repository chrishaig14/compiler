//
// Created by chris on 11/4/21.
//

#ifndef XLANG_OPTION_H
#define XLANG_OPTION_H


#include <string>
#include "Exp.h"

class sem::Option : public sem::Exp {
public:
    explicit Option(sem::UExp exp);

    Option(const Option& other) : sem::Exp(ExpType::OPTION) {
        this->exp = other.exp != nullptr ? other.exp->clone() : nullptr;
    }

    sem::UExp exp;

    bool equals(const Exp& o) const override;
    UExp clone() const override;
};


#endif //XLANG_OPTION_H
