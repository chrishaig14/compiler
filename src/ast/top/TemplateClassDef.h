//
// Created by chris on 4/10/20.
//

#ifndef TEMPLATENODE_H
#define TEMPLATENODE_H


#include <map>
#include "../statements/Statement.h"

#include "../general/Type.h"
#include "Function.h"
#include <util/macros.h>
#include "../ast.h"
#include "AstMethod.h"

class ast::TemplateClassDef : public ast::TopNode {
public:

    bool equal(const ast::TopNode& other) const override;
    TemplateClassDef(const std::string& className, VectorOfStrings type_parameters,
                     std::vector<ast::ClassAttribute> attributes, std::vector<std::unique_ptr<AstMethod>> methods,
                     std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_attributes, TextPosition start,
                     TextPosition end);
    ~TemplateClassDef() override;
    nlohmann::json to_json() const override;

    std::vector<ast::ClassAttribute> attributes;
    std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_attributes;
    VectorOfStrings aatributes_ordered;
    std::vector<std::unique_ptr<AstMethod>> methods;


    std::string class_name;
    VectorOfStrings type_parameters;
};


#endif //TEMPLATENODE_H
