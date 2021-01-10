//
// Created by chris on 24/12/20.
//

#include "ImportNode.h"

bool ImportNode::equal(const Node& other) const {
    return false;
}

ImportNode& ImportNode::import() {
    return *this;
}

const ImportNode& ImportNode::import() const {
    return *this;
}
