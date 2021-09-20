//
// Created by chris on 28/4/21.
//

#ifndef XLANG_ALIAS_H
#define XLANG_ALIAS_H


#include "Node.h"
#include "general/Type.h"
#include "ast.h"

class ast::Alias : public ast::Node {
public:
    Alias(std::string alias_id, ast::Type* aliased_type, TextPosition start, TextPosition end);
    bool equal(const ast::Node& other) const override;
    nlohmann::json to_json() const override;

    std::string alias_id;
    ast::Type* aliased_type;
};


#endif //XLANG_ALIAS_H
