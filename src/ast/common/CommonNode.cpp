//
// Created by chris on 1/8/20.
//

#include <stdexcept>
#include "CommonNode.h"

using namespace ast;

bool ast::CommonNode::operator==(const ast::CommonNode& other) const {
    if (this->ntype != other.ntype) {
        return false;
    }
    return this->equal(other);
}

bool ast::CommonNode::operator!=(const ast::CommonNode& other) const {
    return !(this->operator==(other));
}

CommonNode::~CommonNode() {
    // std::cout << "Destructor for " << this << " called" << std::endl;
}

bool ast::CommonNode::equal(const ast::CommonNode& other) const {
    // std::cout << "BASE NODE for " << this << " EQUAL CALLED!" << std::endl;
    return false;
}

CommonNode::CommonNode(CommonNodeType ntype, TextPosition start, TextPosition end) : ntype(ntype) {
    this->start = start;
    this->end = end;
}
