//
// Created by chris on 13/4/21.
//

#include "IfSNode.h"

using namespace sem;

IfSNode::IfSNode(USNode condition, std::unique_ptr<Block> then,
                 std::vector<std::pair<USNode, std::unique_ptr<Block>>> elifs, Block* _else)
        : SNode(SNodeType::IF), condition(std::move(condition)), then(std::move(then)), _else(_else), elifs(std::move(elifs)) {
}
