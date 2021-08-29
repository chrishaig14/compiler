//
// Created by chris on 8/5/21.
//

#ifndef XLANG_TERNARY_H
#define XLANG_TERNARY_H


#include "SNode.h"

class Ternary : public SNode {

public:
    Ternary(SNode* ext, SNode* true_case, SNode* false_case);
    SNode* ext;
    SNode* true_case;
    SNode* false_case;
};


#endif //XLANG_TERNARY_H
