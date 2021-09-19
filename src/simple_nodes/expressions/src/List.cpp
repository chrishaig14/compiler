//
// Created by chris on 12/4/21.
//

#include "../include/List.h"
using namespace sem;
List::List(std::vector<UExp> elements) : Exp(ExpType::LIST), elements(std::move(elements)) {
}

bool List::equals(const Exp& o) const {
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
