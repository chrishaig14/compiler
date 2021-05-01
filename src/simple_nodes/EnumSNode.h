//
// Created by chris on 30/4/21.
//

#ifndef XLANG_ENUMSNODE_H
#define XLANG_ENUMSNODE_H


#include <string>
#include "SNode.h"
#include "../types.h"

class EnumSNode : public SNode {
public:
    EnumSNode();

    std::string id;
    VectorOfStrings values;
};


#endif //XLANG_ENUMSNODE_H
