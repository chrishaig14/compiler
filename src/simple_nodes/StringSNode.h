//
// Created by chris on 5/4/21.
//

#ifndef XLANG_STRINGSNODE_H
#define XLANG_STRINGSNODE_H


#include <string>
#include "SNode.h"

class StringSNode : public SNode {
public:
    std::string s;
    StringSNode();
};


#endif //XLANG_STRINGSNODE_H
