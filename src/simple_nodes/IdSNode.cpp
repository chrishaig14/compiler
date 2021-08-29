//
// Created by chris on 4/4/21.
//

#include "IdSNode.h"
using namespace sem;
IdSNode::IdSNode(const std::string& identifier) : SNode(SNodeType::ID) {
    this->identifier = identifier;
}
