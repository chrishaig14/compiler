//
// Created by chris on 4/4/21.
//

#include "IdSNode.h"

IdSNode::IdSNode() : SNode(SNodeType::ID) {
}

IdSNode::IdSNode(std::string identifier) : SNode(SNodeType::ID) {
    this->identifier = identifier;
}
