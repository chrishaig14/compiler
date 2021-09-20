//
// Created by chris on 26/4/21.
//

#ifndef XLANG_MATCH_H
#define XLANG_MATCH_H


#include "../Node.h"
#include "../general/Type.h"
#include "Block.h"
#include "../ast.h"

class ast::Match : public ast::Node {
public:
    ExpNode* exp;
    std::vector<std::string> ids;
    std::vector<std::pair<ast::UTypeNode , ast::UBlock>> cases;
    Match(ExpNode* exp, std::vector<std::string> ids, std::vector<std::pair<ast::UTypeNode, ast::UBlock>> cases,
          TextPosition start, TextPosition end);

    bool equal(const ast::Node& other) const override;
    nlohmann::json to_json() const override;
};


#endif //XLANG_MATCH_H
