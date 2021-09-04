//
// Created by chris on 1/8/21.
//

#ifndef XLANG_INSTANCE_H
#define XLANG_INSTANCE_H


#include "Node.h"
#include "ObjectType.h"
#include "Function.h"
#include "ast.h"

class ast::Instance : public ast::Node {
public:
    std::string id;
    ast::UObjectType base_type;
    std::unordered_map<std::string, ast::UFunctionNode> methods;
    Instance(const std::string& id, ast::UObjectType base_type,
             std::unordered_map<std::string, ast::UFunctionNode> methods, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;
    bool equal(const ast::Node& other) const override;
};


#endif //XLANG_INSTANCE_H
