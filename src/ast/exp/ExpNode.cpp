//
// Created by chris on 1/8/20.
//

#include <stdexcept>
#include "ExpNode.h"

using namespace ast;

bool ast::ExpNode::operator==(const ast::ExpNode& other) const {
    if (this->ntype != other.ntype) {
        return false;
    }
    return this->equal(other);
}

bool ast::ExpNode::operator!=(const ast::ExpNode& other) const {
    return !(this->operator==(other));
}

ExpNode::~ExpNode() {
    // std::cout << "Destructor for " << this << " called" << std::endl;
}

bool ast::ExpNode::equal(const ast::ExpNode& other) const {
    // std::cout << "BASE NODE for " << this << " EQUAL CALLED!" << std::endl;
    return false;
}

ExpNode::ExpNode(ExpNodeType ntype, TextPosition start, TextPosition end) : ntype(ntype) {
    this->start = start;
    this->end = end;
}
