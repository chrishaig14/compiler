//
// Created by chris on 12/4/21.
//

#ifndef XLANG_LIST_H
#define XLANG_LIST_H


#include <vector>
#include "../../expressions/include/Exp.h"
#include <cassert>

class sem::List : public sem::Exp {

public:
    std::vector<UExp> elements;
    explicit List(std::vector<UExp> elements);

    bool equals(const Exp& o) const override;
};


#endif //XLANG_LIST_H
