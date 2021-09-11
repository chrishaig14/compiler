//
// Created by chris on 1/8/20.
//

#include <stdexcept>
#include "Node.h"

using namespace ast;

bool ast::Node::operator==(const ast::Node& other) const {
    if (this->ntype != other.ntype) {
        return false;
    }
    return this->equal(other);
}

bool ast::Node::operator!=(const ast::Node& other) const {
    return !(this->operator==(other));
}

Node::~Node() {
    // std::cout << "Destructor for " << this << " called" << std::endl;
}

bool ast::Node::equal(const ast::Node& other) const {
    // std::cout << "BASE NODE for " << this << " EQUAL CALLED!" << std::endl;
    return false;
}

Node::Node(NodeType ntype, TextPosition start, TextPosition end) : ntype(ntype) {
    this->start = start;
    this->end = end;
}
