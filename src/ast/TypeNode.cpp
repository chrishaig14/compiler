//
// Created by chris on 1/8/20.
//

#include <cassert>
#include "TypeNode.h"

using namespace ast;

bool TypeNode::operator!=(const ast::TypeNode& other) const {
    return !(*this == other);
}

bool TypeNode::operator==(const ast::TypeNode& other) const {
    if (this->kind != other.kind) {
        return false;
    }
    return this->equal(other);
}

ast::ObjectType& TypeNode::object() {
    throw std::runtime_error("Getting wrong type!");
}

ast::FunctionType& TypeNode::function() {
    throw std::runtime_error("Getting wrong type!");
}

const ast::FunctionType& TypeNode::function() const {
    throw std::runtime_error("Getting wrong type!");
}

const ast::ObjectType& TypeNode::object() const {
    throw std::runtime_error("Getting wrong type!");
}

bool TypeNode::is_generic() const {
    return false;
}

std::ostream& operator<<(std::ostream& o, const ast::TypeNode& t) {
    o << t.to_string();
    return o;
}
