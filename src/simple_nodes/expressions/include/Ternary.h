//
// Created by chris on 8/5/21.
//

#ifndef XLANG_TERNARY_H
#define XLANG_TERNARY_H


#include "../../expressions/include/Exp.h"

class sem::Ternary : public sem::Exp {

public:
    Ternary(Exp* ext, Exp* true_case, Exp* false_case);
    Exp* ext;
    Exp* true_case;
    Exp* false_case;

    bool equals(const Exp& o) const override {
        // auto& other = (const Ternary&) o;
        return false;
    }
};


#endif //XLANG_TERNARY_H
