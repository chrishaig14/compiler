//
// Created by chris on 18/8/20.
//

#ifndef BREAKNODE_H
#define BREAKNODE_H


#include "CommonNode.h"
#include "../general/Type.h"

#include "../ast.h"

class ast::Break : public ast::CommonNode {
public:
    std::vector<std::pair<std::string, ast::Type*>> loop_vars;
    Break(TextPosition start, TextPosition end);
    bool equal(const ast::CommonNode& other) const override;
    nlohmann::json to_json() const override;

};


#endif //BREAKNODE_H
