//
// Created by chris on 10/4/21.
//

#include "WhileSNode.h"

WhileSNode::WhileSNode(USNode condition, BlockSNode* body)
        : SNode(SNodeType::WHILE), condition(std::move(condition)), body(body) {
}
