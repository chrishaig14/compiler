//
// Created by chris on 1/8/20.
//

#include <stdexcept>
#include "TopNode.h"

using namespace ast;

bool ast::TopNode::operator==(const ast::TopNode& other) const {
    if (this->ntype != other.ntype) {
        return false;
    }
    return this->equal(other);
}

bool ast::TopNode::operator!=(const ast::TopNode& other) const {
    return !(this->operator==(other));
}

TopNode::~TopNode() {
    // std::cout << "Destructor for " << this << " called" << std::endl;
}

bool ast::TopNode::equal(const ast::TopNode& other) const {
    // std::cout << "BASE NODE for " << this << " EQUAL CALLED!" << std::endl;
    return false;
}

TopNode::TopNode(TopNodeType ntype, TextPosition start, TextPosition end) : ntype(ntype) {
    this->start = start;
    this->end = end;
}
