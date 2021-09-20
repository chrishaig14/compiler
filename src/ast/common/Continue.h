//
// Created by chris on 7/11/20.
//

#ifndef CONTINUENODE_H
#define CONTINUENODE_H


#include "CommonNode.h"
#include "../ast.h"

class ast::Continue : public ast::CommonNode {
public:
    Continue(TextPosition start, TextPosition end);
    bool equal(const ast::CommonNode& other) const override;
    nlohmann::json to_json() const override;
};


#endif //CONTINUENODE_H
