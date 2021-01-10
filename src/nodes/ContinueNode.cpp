//
// Created by chris on 7/11/20.
//

#include "ContinueNode.h"

bool ContinueNode::equal(const Node& other) const {
    return true;
}

ContinueNode& ContinueNode::cntinue() {
    return *this;
}

const ContinueNode& ContinueNode::cntinue() const {
    return *this;
}

ContinueNode::ContinueNode() {
    this->ntype = CNTINUE;
}

