//
// Created by chris on 29/4/21.
//

#ifndef XLANG_ENUMNODE_H
#define XLANG_ENUMNODE_H


#include "Node.h"
#include "../types.h"
#include "ast.h"
#include "TopNode.h"

class ast::EnumNode : public ast::TopNode {
public:

    EnumNode(std::string id, VectorOfStrings values, TextPosition start, TextPosition end);
    bool equal(const ast::TopNode& other) const override;
    nlohmann::json to_json() const override;

    VectorOfStrings values;
    std::string id;
};


#endif //XLANG_ENUMNODE_H
