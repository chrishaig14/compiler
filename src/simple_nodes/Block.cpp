//
// Created by chris on 4/4/21.
//

#include "Block.h"

Block::Block(bool unwrap) : SNode(SNodeType::BLOCK) {
    this->unwrap = unwrap;
}

Block::Block() : SNode(SNodeType::BLOCK) {
    this->unwrap = false;
}
