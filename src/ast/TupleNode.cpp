//
// Created by chris on 23/11/20.
//

#include "TupleNode.h"

bool TupleNode::equal(const Node& n) const {
    auto& other = (TupleNode&)n;
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

TupleNode::TupleNode(const VectorOfNodes& values, TextPosition start, TextPosition end):Node(NodeType::TUPLE, start, end) {
    this->values = values;
}

nlohmann::json TupleNode::to_json() const {
    return nlohmann::json();
}

