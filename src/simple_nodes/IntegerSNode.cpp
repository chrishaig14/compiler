//
// Created by chris on 4/4/21.
//

#include <iostream>
#include "IntegerSNode.h"
using namespace sem;
IntegerSNode::IntegerSNode(const std::string& str) : SNode(SNodeType::INTEGER), str(str) {
}
