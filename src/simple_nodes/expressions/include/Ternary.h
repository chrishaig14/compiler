//
// Created by chris on 8/5/21.
//

#ifndef XLANG_TERNARY_H
#define XLANG_TERNARY_H


#include "Exp.h"

class sem::Ternary : public sem::Exp {

public:
    Ternary(UExp exp, UExp true_case, UExp false_case);
    UExp exp;
    UExp true_case;
    UExp false_case;

    Ternary(const Ternary& other);

    bool equals(const Exp& o) const override;
    UExp clone() const override;
};


#endif //XLANG_TERNARY_H
