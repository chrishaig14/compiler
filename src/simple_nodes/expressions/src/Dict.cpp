//
// Created by chris on 9/5/21.
//

#include "../include/Dict.h"

using namespace sem;

Dict::Dict(std::vector<std::pair<USNode, USNode>> items) : SNode(SNodeType::DICT), items(std::move(items)) {
}
