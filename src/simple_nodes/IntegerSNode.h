//
// Created by chris on 4/4/21.
//

#ifndef XLANG_INTEGERSNODE_H
#define XLANG_INTEGERSNODE_H


#include <string>
#include "SNode.h"

class sem::IntegerSNode : public sem::SNode {
public:
    explicit IntegerSNode(const std::string& str);
    std::string str;
};


#endif //XLANG_INTEGERSNODE_H
