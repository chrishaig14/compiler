//
// Created by chris on 11/4/21.
//

#ifndef XLANG_NEWOBJECTSNODE_H
#define XLANG_NEWOBJECTSNODE_H


#include <string>
#include <vector>
#include "SNode.h"

class sem::NewObjectSNode : public sem::SNode {
public:
    std::string class_name;
    std::vector<SNode*> args;

    NewObjectSNode() : SNode(SNodeType::NEW) {

    }
};


#endif //XLANG_NEWOBJECTSNODE_H
