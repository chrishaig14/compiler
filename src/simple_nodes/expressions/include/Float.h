//
// Created by chris on 11/4/21.
//

#ifndef XLANG_FLOAT_H
#define XLANG_FLOAT_H


#include <string>
#include "../../expressions/include/Exp.h"

class sem::Float : public sem::Exp {
public:
    Float(const std::string& s);

    std::string str;

    bool equals(const Exp& o) const override {
        auto& other = (const Float&) o;
        return this->str == other.str;
    }
};


#endif //XLANG_FLOAT_H
