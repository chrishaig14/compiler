//
// Created by chris on 30/8/20.
//

#ifndef NONENODE_H
#define NONENODE_H


#include "ExpNode.h"

#include "../ast.h"

class ast::None : public ast::ExpNode {
public:
    None(TextPosition start, TextPosition end);
    bool equal(const ast::ExpNode& other) const override;
    nlohmann::json to_json() const override;

};


#endif //NONENODE_H
