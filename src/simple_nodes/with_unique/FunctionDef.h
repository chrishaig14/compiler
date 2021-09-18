//
// Created by chris on 4/4/21.
//

#ifndef XLANG_FUNCTIONDEF_H
#define XLANG_FUNCTIONDEF_H

#include <string>
#include "../common/include/SNode.h"
#include "../../types.h"
#include "../common/include/Block.h"

class sem::FunctionDef : public sem::SNode {
public:
    std::string identifier;
    VectorOfStrings params;
    std::unique_ptr<Block> body;

    FunctionDef(std::string identifier, VectorOfStrings params, std::unique_ptr<Block> body);

    bool equals(const SNode& o) const override {
        auto& other = (const FunctionDef&) o;
        bool id_ok = this->identifier == other.identifier;
        bool params_ok = this->params == other.params;
        bool body_ok = *this->body == *other.body;
        return id_ok && params_ok && body_ok;
    }
};


#endif //XLANG_FUNCTIONDEF_H
