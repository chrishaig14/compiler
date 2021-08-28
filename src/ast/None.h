//
// Created by chris on 30/8/20.
//

#ifndef NONENODE_H
#define NONENODE_H


#include "Node.h"

#include "ast.h"
class ast::None : public ast::Node {
public:
    None(TextPosition start, TextPosition end);
    bool equal(const ast::Node& other) const override;
    nlohmann::json to_json() const override;

};


#endif //NONENODE_H
