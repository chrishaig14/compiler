//
// Created by chris on 4/4/21.
//

#include "BlockSNode.h"

BlockSNode::BlockSNode(bool unwrap) : SNode(SNodeType::BLOCK) {
    this->unwrap = unwrap;
}

BlockSNode::BlockSNode() : SNode(SNodeType::BLOCK) {
    this->unwrap = false;
}
