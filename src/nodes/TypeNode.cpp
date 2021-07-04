//
// Created by chris on 1/8/20.
//

#include <cassert>
#include "TypeNode.h"


bool TypeNode::operator!=(const TypeNode& other) const {
    return !(*this == other);
}

bool TypeNode::operator==(const TypeNode& other) const {
    if (this->kind != other.kind) {
        return false;
    }
    return this->equal(other);
}

ObjectType& TypeNode::object() {
    throw std::runtime_error("Getting wrong type!");
}

FunctionType& TypeNode::function() {
    throw std::runtime_error("Getting wrong type!");
}

const FunctionType& TypeNode::function() const {
    throw std::runtime_error("Getting wrong type!");
}

const ObjectType& TypeNode::object() const {
    throw std::runtime_error("Getting wrong type!");
}

bool TypeNode::is_generic() const {
    return false;
}
