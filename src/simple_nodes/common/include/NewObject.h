//
// Created by chris on 11/4/21.
//

#ifndef XLANG_NEWOBJECT_H
#define XLANG_NEWOBJECT_H


#include <string>
#include <vector>
#include "SNode.h"

class sem::NewObject : public sem::SNode {
public:
    std::string class_name;
    std::vector<USNode> args;

    NewObject() : SNode(SNodeType::NEW) {

    }

    bool equals(const SNode& o) const override {
        return false;
    }
};


#endif //XLANG_NEWOBJECT_H
