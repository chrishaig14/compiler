//
// Created by chris on 24/12/20.
//

#ifndef UNTITLED1_IMPORTNODE_H
#define UNTITLED1_IMPORTNODE_H


#include "TopNode.h"

#include <utility>
#include <util/types.h>
#include <common/Token.h>
#include "../ast.h"
#include "TopNode.h"

class ast::Import : public ast::TopNode {
public:
    bool has_alias;
    bool equal(const ast::TopNode& other) const override;

    Import(std::vector<Token> path_parts, std::string alias, TextPosition start, TextPosition end);

    Import(std::vector<Token> path_parts);

    nlohmann::json to_json() const override;

    std::vector<Token> path;
    std::string alias;
};


#endif //UNTITLED1_IMPORTNODE_H
