//
// Created by chris on 8/5/21.
//

#ifndef XLANG_NONE_H
#define XLANG_NONE_H


#include "../../expressions/include/Exp.h"

class sem::None : public sem::Exp {
public:
    None();

    bool equals(const Exp& o) const override {
        return true;
    }

};


#endif //XLANG_NONE_H
