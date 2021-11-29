//
// Created by chris on 1/8/21.
//

#ifndef XLANG_INSTANCEAST_H
#define XLANG_INSTANCEAST_H


#include "../statements/Statement.h"
#include "../general/ObjectType.h"
#include "Function.h"
#include "../ast.h"

class ast::Instance : public ast::TopNode {
public:
    std::string id;
    ast::UObjectType base_type;
    std::unordered_map<std::string, ast::UFunctionNode> methods;
    std::unordered_map<std::string, ast::UFunctionNode> static_methods;
    Instance(const std::string& id, ast::UObjectType base_type,
             std::unordered_map<std::string, ast::UFunctionNode> methods,std::unordered_map<std::string, ast::UFunctionNode> static_methods, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;
    bool equal(const ast::TopNode& other) const override;
};


#endif //XLANG_INSTANCEAST_H
