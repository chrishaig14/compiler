//
// Created by chris on 10/4/21.
//

#include "WhileSNode.h"

WhileSNode::WhileSNode(SNode* condition, BlockSNode* body) : SNode(SNodeType::WHILE), condition(condition), body(body) {
}
