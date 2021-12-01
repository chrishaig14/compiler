//
// Created by chris on 9/5/21.
//

#include "../include/Dict.h"

using namespace sem;

Dict::Dict(std::vector<std::pair<UExp, UExp>> items) : Exp(ExpType::DICT), items(std::move(items)) {
}

bool Dict::equals(const Exp& o) const {
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

UExp Dict::clone() const {
    return std::make_unique<Dict>(*this);
}

Dict::Dict(const Dict& other) : sem::Exp(ExpType::DICT) {
    for (auto& i : other.items) {
        this->items.emplace_back(i.first->clone(), i.second->clone());
    }
}
