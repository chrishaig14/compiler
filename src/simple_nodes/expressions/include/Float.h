//
// Created by chris on 11/4/21.
//

#ifndef XLANG_FLOAT_H
#define XLANG_FLOAT_H


#include <string>
#include "Exp.h"

class sem::Float : public sem::Exp {
public:
    Float(const std::string& s);

    std::string str;

    bool equals(const Exp& o) const override;
    UExp clone() const override;
};


#endif //XLANG_FLOAT_H
