//
// Created by chris on 19/6/21.
//

#ifndef XLANG_THROW_H
#define XLANG_THROW_H


#include "SNode.h"
#include <vector>
#include <string>

class Throw : public SNode {
public:
    SNode* expression;
    std::vector<std::string> reachables;
    Throw(SNode* expression);
};


#endif //XLANG_THROW_H
