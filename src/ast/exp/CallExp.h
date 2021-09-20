//
// Created by chris on 1/8/20.
//

#ifndef CallExpNODE_H
#define CallExpNODE_H


#include <vector>
#include "../Node.h"
#include "../../types.h"
#include "../Type.h"
#include "../ast.h"
#include "ExpNode.h"

class ast::CallExp : public ast::ExpNode {
    ast::UExpNode _function;
    ast::VectorOfExpNodesU _arguments;
public:
    ExpNode& function;
    std::vector<RExpNode> arguments;

    CallExp(ast::UExpNode function, ast::VectorOfExpNodesU arguments, TextPosition start, TextPosition end);

    bool equal(const ast::ExpNode& other) const override;

    std::unique_ptr<ast::Call> to_call();

    ~CallExp();
    nlohmann::json to_json() const override;

    ast::VectorOfTypes ftype;
    ast::VectorOfTypes arg_types;
};


#endif //CallExpNODE_H
