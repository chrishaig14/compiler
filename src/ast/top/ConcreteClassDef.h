//
// Created by chris on 4/10/20.
//

#ifndef CLASSNODE_H
#define CLASSNODE_H


#include <map>
#include "../statements/Statement.h"

#include "../general/Type.h"
#include "Function.h"
#include <util/macros.h>
#include "../ast.h"
#include "AstMethod.h"


class ast::ConcreteClassDef : public ast::TopNode {
public:

    bool equal(const ast::TopNode& other) const override;
    ConcreteClassDef(const std::string& className, std::vector<ClassAttribute> attributes,
                     std::vector<std::unique_ptr<AstMethod>> methods,
                     std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_attributes, TextPosition start,
                     TextPosition end);
    ~ConcreteClassDef() override;
    nlohmann::json to_json() const override;

    std::vector<ast::ClassAttribute> attributes;
    std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_attributes;
    VectorOfStrings attributes_ordered;
    std::vector<std::unique_ptr<AstMethod>> methods;

    std::string class_name;
};


#endif //CLASSNODE_H
