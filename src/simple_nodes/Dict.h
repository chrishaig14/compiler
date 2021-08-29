//
// Created by chris on 9/5/21.
//

#ifndef XLANG_DICT_H
#define XLANG_DICT_H


#include <vector>
#include "SNode.h"

class sem::Dict : public sem::SNode {

public:
    explicit Dict(const std::vector<std::pair<SNode*, SNode*>>& items);
    std::vector<std::pair<SNode*, SNode*>> items;
    bool equals(const SNode& o) const override {
        // auto& other = (const Declaration&) o;
        // return this->identifier == other.identifier && *this->expression == *other.expression;
        return false;
    }
};


#endif //XLANG_DICT_H
