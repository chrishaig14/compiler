//
// Created by chris on 9/5/21.
//

#ifndef XLANG_DICT_H
#define XLANG_DICT_H


#include <vector>
#include "../SNode.h"

class sem::Dict : public sem::SNode {

public:
    explicit Dict(std::vector<std::pair<USNode, USNode>> items);
    std::vector<std::pair<USNode, USNode>> items;

    bool equals(const SNode& o) const override {
        auto& other = (const Dict&) o;
        // return this->identifier == other.identifier && *this->expression == *other.expression;
        if (this->items.size() != other.items.size()) {
            return false;
        }
        for (size_t i = 0; i < this->items.size(); i++) {
            if (*this->items[i].first != *other.items[i].first || *this->items[i].second != *other.items[i].second) {
                return false;
            }
        }
        return true;
    }
};


#endif //XLANG_DICT_H
