//
// Created by chris on 9/5/21.
//

#include "Dict.h"
using namespace sem;
Dict::Dict(const std::vector<std::pair<SNode*, SNode*>>& items) : SNode(SNodeType::DICT) {
    this->items = items;
}
