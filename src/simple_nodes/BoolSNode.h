//
// Created by chris on 11/4/21.
//

#ifndef XLANG_BOOLSNODE_H
#define XLANG_BOOLSNODE_H


#include "SNode.h"

class BoolSNode : public SNode {

public:
    explicit BoolSNode(bool v);
    bool v;
};


#endif //XLANG_BOOLSNODE_H
