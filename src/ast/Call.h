//
// Created by chris on 1/8/20.
//

#ifndef CALLNODE_H
#define CALLNODE_H


#include <vector>
#include "Node.h"
#include "../types.h"
#include "TypeNode.h"
#include "ast.h"

class ast::Call : public ast::Node {
    ast::UNode _function;
    ast::VectorOfNodesU _arguments;
public:
    Node& function;
    std::vector<RNode> arguments;

    Call(ast::UNode function, ast::VectorOfNodesU arguments, TextPosition start, TextPosition end);

    bool equal(const ast::Node& other) const override;

    ~Call();
    nlohmann::json to_json() const override;

    VectorOfTypes ftype;
    VectorOfTypes arg_types;
};


#endif //CALLNODE_H
