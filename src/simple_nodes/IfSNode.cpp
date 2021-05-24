//
// Created by chris on 13/4/21.
//

#include "IfSNode.h"

IfSNode::IfSNode(SNode* condition, BlockSNode* then, std::vector<std::pair<SNode*, BlockSNode*>> elifs,
                 BlockSNode* _else)
        : SNode(SNodeType::IF), condition(condition), then(then), _else(_else), elifs(elifs) {
}
