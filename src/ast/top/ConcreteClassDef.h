//
// Created by chris on 4/10/20.
//

#ifndef CLASSNODE_H
#define CLASSNODE_H


#include <map>
#include "../statements/Statement.h"

#include "../general/Type.h"
#include "Function.h"
#include "../../util/macros.h"
#include "../ast.h"

class ast::ConcreteClassDef : public ast::TopNode {
public:

    bool equal(const ast::TopNode& other) const override;
    ConcreteClassDef(const std::string& className, std::vector<std::pair<std::string, ast::UTypeNode>> members,
                     std::unordered_map<std::string, ast::UFunctionNode> functions,
                     std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_members,
                     std::unordered_map<std::string, ast::UFunctionNode>& static_methods, TextPosition start,
                     TextPosition end);
    ~ConcreteClassDef() override;
    nlohmann::json to_json() const override;

    std::vector<std::pair<std::string, ast::UTypeNode>> members;
    std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_members;
    VectorOfStrings members_ordered;
    std::unordered_map<std::string, ast::UFunctionNode> methods;
    std::unordered_map<std::string, ast::UFunctionNode> static_methods;

    std::string class_name;
};


#endif //CLASSNODE_H
