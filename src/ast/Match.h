//
// Created by chris on 26/4/21.
//

#ifndef XLANG_MATCH_H
#define XLANG_MATCH_H


#include "Node.h"
#include "TypeNode.h"
#include "Block.h"
#include "ast.h"

class ast::Match : public ast::Node {
public:
    Node* exp;
    std::vector<std::string> ids;
    std::vector<std::pair<UTypeNode , UBlockNode>> cases;
    Match(Node* exp, std::vector<std::string> ids, std::vector<std::pair<UTypeNode, UBlockNode>> cases,
          TextPosition start, TextPosition end);

    bool equal(const ast::Node& other) const override;
    nlohmann::json to_json() const override;
};


#endif //XLANG_MATCH_H
