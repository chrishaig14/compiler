//
// Created by chris on 5/9/21.
//

#ifndef XLANG_AST_MODULE_H
#define XLANG_AST_MODULE_H

#include "../ast.h"
#include <vector>
#include <memory>
#include <iostream>

class ast::Module {
public:
    std::vector<std::reference_wrapper<ast::Import>> imports;
    std::vector<std::reference_wrapper<ast::ConcreteClassDef>> classes;
    std::vector<std::reference_wrapper<ast::TemplateClassDef>> template_classes;
    std::vector<std::reference_wrapper<ast::EnumNode>> enums;
    std::vector<std::reference_wrapper<ast::Function>> functions;
    std::vector<std::reference_wrapper<ast::TypeclassAst>> typeclasses;
    std::vector<std::reference_wrapper<ast::Instance>> instances;

    Module(std::vector<std::unique_ptr<ast::TopNode>> all, std::vector<std::reference_wrapper<ast::Import>> imports,
           std::vector<std::reference_wrapper<ast::ConcreteClassDef>> classes,
           std::vector<std::reference_wrapper<ast::TemplateClassDef>> template_classes,
           std::vector<std::reference_wrapper<ast::EnumNode>> enums,
           std::vector<std::reference_wrapper<ast::Function>> functions,
           std::vector<std::reference_wrapper<ast::TypeclassAst>> typeclasses,
           std::vector<std::reference_wrapper<ast::Instance>> instances
           );
    std::vector<std::unique_ptr<ast::TopNode>> all;
};


#endif //XLANG_AST_MODULE_H
