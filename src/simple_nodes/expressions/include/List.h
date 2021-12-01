//
// Created by chris on 12/4/21.
//

#ifndef XLANG_LIST_H
#define XLANG_LIST_H


#include <vector>
#include "Exp.h"
#include <cassert>

class sem::List : public sem::Exp {

public:
    std::vector<UExp> elements;
    explicit List(std::vector<UExp> elements);

    List(const List& other) : sem::Exp(ExpType::LIST) {
        for (auto& e: other.elements) {
            this->elements.emplace_back(e->clone());
        }
    }

    bool equals(const Exp& o) const override;
    UExp clone() const override;
};


#endif //XLANG_LIST_H
