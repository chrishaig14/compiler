//
// Created by chris on 30/8/20.
//

#include "NoneNode.h"

bool NoneNode::equal(const Node& other) const {
    return true;
}

NoneNode& NoneNode::none() {
    return *this;
}

const NoneNode& NoneNode::none() const {
    return *this;
}

NoneNode::NoneNode(TextPosition start, TextPosition end):Node(NONE, start, end) {
}

