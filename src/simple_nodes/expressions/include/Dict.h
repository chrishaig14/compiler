//
// Created by chris on 9/5/21.
//

#ifndef XLANG_DICT_H
#define XLANG_DICT_H


#include <vector>
#include "Exp.h"

class sem::Dict : public sem::Exp {

public:
    explicit Dict(std::vector<std::pair<UExp, UExp>> items);
    std::vector<std::pair<UExp, UExp>> items;

    Dict(const Dict& other);

    bool equals(const Exp& o) const override;
    UExp clone() const override;
};


#endif //XLANG_DICT_H
