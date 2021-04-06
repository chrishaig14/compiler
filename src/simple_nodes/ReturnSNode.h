//
// Created by chris on 4/4/21.
//

#ifndef XLANG_RETURNSNODE_H
#define XLANG_RETURNSNODE_H


#include "SNode.h"

class ReturnSNode : public SNode {
public:
    SNode* expression;

    ReturnSNode();
};


#endif //XLANG_RETURNSNODE_H
