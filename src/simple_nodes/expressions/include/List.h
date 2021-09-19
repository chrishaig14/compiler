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

    bool equals(const Exp& o) const override {
        assert(this->type == o.type);
        auto& other = (const List&) o;
        if (this->elements.size() != other.elements.size()) {
            return false;
        }
        for (size_t i = 0; i < this->elements.size(); i++) {
            if (*this->elements[i] != *other.elements[i]) {
                return false;
            }
        }
        return true;
    }
};


#endif //XLANG_LIST_H
