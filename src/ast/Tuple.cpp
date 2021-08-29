//
// Created by chris on 23/11/20.
//

#include "Tuple.h"

using namespace ast;

bool Tuple::equal(const ast::Node& n) const {
    auto& other = (Tuple&) n;
    if (this->values.size() != other.values.size()) {
        return false;
    }
    for (size_t i = 0; i < this->values.size(); i++) {
        if (*this->values[i] != *other.values[i]) {
            return false;
        }
    }
    return true;
}

Tuple::Tuple(VectorOfNodesU values, TextPosition start, TextPosition end) : Node(NodeType::TUPLE, start, end),
                                                                            values(std::move(values)) {
}

nlohmann::json Tuple::to_json() const {
    return nlohmann::json();
}

