//
// Created by chris on 11/4/21.
//

#ifndef XLANG_KLASS_H
#define XLANG_KLASS_H


#include <string>
#include "SNode.h"
#include "FunctionSNode.h"

class sem::Klass : public sem::SNode {
public:
    std::string identifier;
    VectorOfStrings members;

    Klass(std::string identifier, VectorOfStrings members)
            : SNode(SNodeType::CLASS), identifier(identifier), members(members) {
    }
};


#endif //XLANG_KLASS_H
