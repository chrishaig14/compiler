//
// Created by chris on 1/8/20.
//

#include <cassert>
#include "Type.h"

using namespace ast;

bool Type::operator!=(const ast::Type& other) const {
    return !(*this == other);
}

bool Type::operator==(const ast::Type& other) const {
    if (this->kind != other.kind) {
        return false;
    }
    return this->equal(other);
}

ast::ObjectType& Type::object() {
    throw std::runtime_error("Getting wrong type!");
}

ast::FunctionType& Type::function() {
    throw std::runtime_error("Getting wrong type!");
}

const ast::FunctionType& Type::function() const {
    throw std::runtime_error("Getting wrong type!");
}

const ast::ObjectType& Type::object() const {
    throw std::runtime_error("Getting wrong type!");
}

bool Type::is_generic() const {
    return false;
}

Type::Type(Kind kind) : kind(kind) {

}

std::ostream& operator<<(std::ostream& o, const ast::Type& t) {
    o << t.to_string();
    return o;
}
