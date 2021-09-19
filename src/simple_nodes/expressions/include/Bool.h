//
// Created by chris on 11/4/21.
//

#ifndef XLANG_BOOL_H
#define XLANG_BOOL_H


#include "../../expressions/include/Exp.h"

class sem::Bool : public sem::Exp {

public:
    explicit Bool(bool v);
    bool v;

    bool equals(const Exp& o) const override {
        auto& other = (const Bool&) o;
        return this->v == other.v;
    }
};


#endif //XLANG_BOOL_H
