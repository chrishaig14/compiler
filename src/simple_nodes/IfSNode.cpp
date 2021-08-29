//
// Created by chris on 13/4/21.
//

#include "IfSNode.h"
using namespace sem;
IfSNode::IfSNode(SNode* condition, Block* then, std::vector<std::pair<SNode*, Block*>> elifs,
                 Block* _else)
        : SNode(SNodeType::IF), condition(condition), then(then), _else(_else), elifs(elifs) {
}
