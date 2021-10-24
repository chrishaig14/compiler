//
// Created by chris on 1/8/20.
//

#ifndef LISTNODE_H
#define LISTNODE_H


#include <vector>
#include "ExpNode.h"
#include <util/types.h>
#include "../../ast.h"

class ast::List : public ast::ExpNode {
    ast::VectorOfExpNodesU _elements;
public:
    std::vector<RExpNode> elements;
    List(ast::VectorOfExpNodesU elements, TextPosition start, TextPosition end);

    bool equal(const ast::ExpNode& other) const override;

    ~List() override;
    nlohmann::json to_json() const override;

    ast::Type* type;
};


#endif //LISTNODE_H
