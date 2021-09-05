//
// Created by chris on 4/10/20.
//

#ifndef CLASSNODE_H
#define CLASSNODE_H


#include <map>
#include "Node.h"

#include "Type.h"
#include "Function.h"
#include "../macros.h"
#include "ast.h"

class KMethod {
public:
    Implicit* constraint;
    ast::UFunctionNode method;

    KMethod(Implicit* constraint, ast::UFunctionNode method);
};

class ast::Klass : public ast::Node {
public:

    bool equal(const ast::Node& other) const override;
    Klass(const std::string& className, VectorOfStrings type_parameters,
          std::vector<std::pair<std::string, ast::UTypeNode>> members, std::unordered_map<std::string, std::unique_ptr<KMethod>> functions,
          std::map<std::string, std::pair<ast::Type*, ast::Node*>> static_members,
          std::unordered_map<std::string, ast::UFunctionNode>& static_methods, TextPosition start, TextPosition end);
    ~Klass() override;
    nlohmann::json to_json() const override;

    std::vector<std::pair<std::string, ast::UTypeNode>> members;
    std::map<std::string, std::pair<ast::Type*, ast::Node*>> static_members;
    VectorOfStrings members_ordered;
    std::unordered_map<std::string, std::unique_ptr<KMethod>> methods;
    std::unordered_map<std::string, ast::UFunctionNode> static_methods;

    std::string class_name;
    VectorOfStrings type_parameters;
};


#endif //CLASSNODE_H
