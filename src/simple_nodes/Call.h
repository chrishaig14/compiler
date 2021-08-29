//
// Created by chris on 4/4/21.
//

#ifndef XLANG_CALL_H
#define XLANG_CALL_H


#include <vector>
#include "SNode.h"

class Call : public SNode {
public:
    SNode* function;
    std::vector<SNode*> arguments;
    Call(SNode* function, std::vector<SNode*> arguments);
};


#endif //XLANG_CALL_H
