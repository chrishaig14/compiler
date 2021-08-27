//
// Created by chris on 28/4/21.
//

#ifndef XLANG_ALIASNODE_H
#define XLANG_ALIASNODE_H


#include "Node.h"
#include "TypeNode.h"
#include "ast.h"
class ast::AliasNode : public ast::Node {
public:
    AliasNode(std::string alias_id, TypeNode* aliased_type, TextPosition start, TextPosition end);
    bool equal(const ast::Node& other) const override;
    nlohmann::json to_json() const override;

    std::string alias_id;
    TypeNode* aliased_type;
};


#endif //XLANG_ALIASNODE_H
