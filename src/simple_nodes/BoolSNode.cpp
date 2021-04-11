//
// Created by chris on 11/4/21.
//

#include "BoolSNode.h"

BoolSNode::BoolSNode(bool v) : SNode(SNodeType::BOOLEAN) {
    this->v = v;
}
