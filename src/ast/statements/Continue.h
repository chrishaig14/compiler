//
// Created by chris on 7/11/20.
//

#ifndef CONTINUENODE_H
#define CONTINUENODE_H


#include "Statement.h"
#include "../ast.h"

class ast::Continue : public ast::Statement {
public:
    Continue(TextPosition start, TextPosition end);
    bool equal(const ast::Statement& other) const override;
    nlohmann::json to_json() const override;
};


#endif //CONTINUENODE_H
