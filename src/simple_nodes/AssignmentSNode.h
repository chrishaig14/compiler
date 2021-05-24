//
// Created by chris on 4/4/21.
//

#ifndef XLANG_ASSIGNMENTSNODE_H
#define XLANG_ASSIGNMENTSNODE_H

#include "SNode.h"

class AssignmentSNode : public SNode {
public:
    SNode* lvalue;
    SNode* rvalue;
    AssignmentSNode(SNode* lvalue, SNode* rvalue);
};


#endif //XLANG_ASSIGNMENTSNODE_H
