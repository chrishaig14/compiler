//
// Created by chris on 11/4/21.
//

#ifndef XLANG_KLASSDEF_H
#define XLANG_KLASSDEF_H


#include <string>
#include "SNode.h"
#include "FunctionDef.h"

class sem::KlassDef : public sem::SNode {
public:
    std::string identifier;
    VectorOfStrings members;

    KlassDef(std::string identifier, VectorOfStrings members)
            : SNode(SNodeType::CLASS), identifier(identifier), members(members) {
    }
};


#endif //XLANG_KLASSDEF_H
