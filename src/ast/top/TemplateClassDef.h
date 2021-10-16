//
// Created by chris on 4/10/20.
//

#ifndef TEMPLATENODE_H
#define TEMPLATENODE_H


#include <map>
#include "../statements/Statement.h"

#include "../general/Type.h"
#include "Function.h"
#include "../../util/macros.h"
#include "../ast.h"

class ast::TemplateClassDef : public ast::TopNode {
public:

    bool equal(const ast::TopNode& other) const override;
    TemplateClassDef(const std::string& className, VectorOfStrings type_parameters,
                     std::vector<std::pair<std::string, ast::UTypeNode>> members,
                     std::unordered_map<std::string, ast::UFunctionNode> functions,
                     std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_members,
                     std::unordered_map<std::string, ast::UFunctionNode>& static_methods, TextPosition start,
                     TextPosition end);
    ~TemplateClassDef() override;
    nlohmann::json to_json() const override;

    std::vector<std::pair<std::string, ast::UTypeNode>> members;
    std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_members;
    VectorOfStrings members_ordered;
    std::unordered_map<std::string, ast::UFunctionNode> methods;
    std::unordered_map<std::string, ast::UFunctionNode> static_methods;

    std::string class_name;
    VectorOfStrings type_parameters;
};


#endif //TEMPLATENODE_H
