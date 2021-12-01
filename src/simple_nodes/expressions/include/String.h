//
// Created by chris on 5/4/21.
//

#ifndef XLANG_STRING_H
#define XLANG_STRING_H


#include <string>
#include "Exp.h"


class sem::String : public sem::Exp {
public:
    std::string s;
    String(const std::string& s);

    bool equals(const Exp& o) const override;
    UExp clone() const override;

};


#endif //XLANG_STRING_H
