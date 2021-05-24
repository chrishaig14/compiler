//
// Created by chris on 4/4/21.
//

#include "AssignmentSNode.h"

AssignmentSNode::AssignmentSNode(SNode* lvalue, SNode* rvalue)
        : SNode(SNodeType::ASSIGNMENT), lvalue(lvalue), rvalue(rvalue) {}
