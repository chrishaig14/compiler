//
// Created by chris on 4/4/21.
//

#include "AssignmentSNode.h"

AssignmentSNode::AssignmentSNode(USNode& lvalue, USNode& rvalue)
        : SNode(SNodeType::ASSIGNMENT), lvalue(std::move(lvalue)), rvalue(std::move(rvalue)) {
}
