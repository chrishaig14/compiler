//
// Created by chris on 6/9/20.
//

#ifndef EMPTYLISTNODE_H
#define EMPTYLISTNODE_H


#include "ExpNode.h"
#include "../Type.h"
#include "../ast.h"

class ast::EmptyList : public ast::ExpNode {
public:
    EmptyList(ast::UTypeNode type, TextPosition start, TextPosition end);

    bool equal(const ast::ExpNode& other) const override;

    ~EmptyList() override;
    nlohmann::json to_json() const override;

    ast::UTypeNode type;
};


#endif //EMPTYLISTNODE_H
