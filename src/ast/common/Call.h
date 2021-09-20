//
// Created by chris on 1/8/20.
//

#ifndef CALLNODE_H
#define CALLNODE_H


#include <vector>
#include "CommonNode.h"
#include "../../util/types.h"
#include "../general/Type.h"
#include "../ast.h"
#include "../exp/include/ExpNode.h"

class ast::Call : public ast::CommonNode {
    ast::UExpNode _function;
    ast::VectorOfExpNodesU _arguments;
public:
    ExpNode& function;
    std::vector<RExpNode> arguments;

    Call(ast::UExpNode function, ast::VectorOfExpNodesU arguments, TextPosition start, TextPosition end);

    bool equal(const ast::CommonNode& other) const override;

    ~Call();
    nlohmann::json to_json() const override;

    ast::VectorOfTypes ftype;
    ast::VectorOfTypes arg_types;
};


#endif //CALLNODE_H
