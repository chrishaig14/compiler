//
// Created by chris on 6/9/20.
//

#ifndef EMPTYLISTNODE_H
#define EMPTYLISTNODE_H


#include "Node.h"
#include "TypeNode.h"
#include "ast.h"

class ast::EmptyList : public ast::Node {
public:
    EmptyList(UTypeNode type, TextPosition start, TextPosition end);

    bool equal(const ast::Node& other) const override;

    ~EmptyList() override;
    nlohmann::json to_json() const override;

    UTypeNode type;
};


#endif //EMPTYLISTNODE_H
