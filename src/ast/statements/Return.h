//
// Created by chris on 1/8/20.
//

#ifndef RETURNNODE_H
#define RETURNNODE_H


#include "Statement.h"
#include <util/types.h>
#include "../general/Type.h"
#include "../ast.h"
#include "../expressions/include/ExpNode.h"

class ast::Return : public ast::Statement {
public:
    explicit Return(ast::UExpNode& expression, TextPosition start, TextPosition end);


    bool equal(const ast::Statement& other) const override;

    ~Return();
    nlohmann::json to_json() const override;

    ast::UExpNode expression;
    std::vector<std::pair<std::string, ast::Type*>> reachables;
};

#endif //RETURNNODE_H
