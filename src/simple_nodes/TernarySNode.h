//
// Created by chris on 8/5/21.
//

#ifndef XLANG_TERNARYSNODE_H
#define XLANG_TERNARYSNODE_H


#include "SNode.h"

class TernarySNode : public SNode {

public:
    TernarySNode(SNode* ext, SNode* true_case, SNode* false_case);
    SNode* ext;
    SNode* true_case;
    SNode* false_case;
};


#endif //XLANG_TERNARYSNODE_H
