//
// Created by chris on 26/4/21.
//

#ifndef XLANG_MATCHEXPRESSIONNODE_H
#define XLANG_MATCHEXPRESSIONNODE_H


#include "Node.h"
#include "TypeNode.h"
#include "Block.h"
#include "ast.h"
class ast::MatchExpressionNode : public ast::Node {
public:
    Node* exp;
    std::vector<std::string> ids;
    std::vector<std::pair<TypeNode*, ast::Block*>> cases;
    MatchExpressionNode(Node* exp, std::vector<std::string> ids, std::vector<std::pair<TypeNode*, ast::Block*>> cases,
                        TextPosition start, TextPosition end);

    bool equal(const ast::Node& other) const override;
    nlohmann::json to_json() const override;
};


#endif //XLANG_MATCHEXPRESSIONNODE_H
