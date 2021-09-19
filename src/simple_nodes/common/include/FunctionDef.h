//
// Created by chris on 4/4/21.
//

#ifndef XLANG_FUNCTIONDEF_H
#define XLANG_FUNCTIONDEF_H

#include <string>
#include "SNode.h"
#include "../../../types.h"
#include "Block.h"

class sem::FunctionDef : public sem::SNode {
public:
    std::string identifier;
    VectorOfStrings params;
    std::unique_ptr<Block> body;

    FunctionDef(std::string identifier, VectorOfStrings params, std::unique_ptr<Block> body);

    bool equals(const SNode& o) const override;
};


#endif //XLANG_FUNCTIONDEF_H
