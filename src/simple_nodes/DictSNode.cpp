//
// Created by chris on 9/5/21.
//

#include "DictSNode.h"
using namespace sem;
DictSNode::DictSNode(const std::vector<std::pair<SNode*, SNode*>>& items) : SNode(SNodeType::DICT) {
    this->items = items;
}
