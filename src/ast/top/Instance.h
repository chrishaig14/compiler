//
// Created by chris on 1/8/21.
//

#ifndef XLANG_INSTANCE_H
#define XLANG_INSTANCE_H


#include "../common/CommonNode.h"
#include "../general/ObjectType.h"
#include "Function.h"
#include "../ast.h"

class ast::Instance : public ast::CommonNode {
public:
    std::string id;
    ast::UObjectType base_type;
    std::unordered_map<std::string, ast::UFunctionNode> methods;
    Instance(const std::string& id, ast::UObjectType base_type,
             std::unordered_map<std::string, ast::UFunctionNode> methods, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;
    bool equal(const ast::CommonNode& other) const override;
};


#endif //XLANG_INSTANCE_H
