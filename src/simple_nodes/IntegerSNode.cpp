//
// Created by chris on 4/4/21.
//

#include <iostream>
#include "IntegerSNode.h"

IntegerSNode::IntegerSNode(std::string str) : SNode(SNodeType::INTEGER), str(str) {
    if (8) std::cout << "Hello";
}
