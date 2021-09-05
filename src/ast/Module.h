//
// Created by chris on 5/9/21.
//

#ifndef XLANG_AST_MODULE_H
#define XLANG_AST_MODULE_H

#include "ast.h"
#include <vector>
#include <memory>
#include <iostream>

class ast::Module {
public:
    std::vector<std::reference_wrapper<ast::Import>> imports;
    std::vector<std::reference_wrapper<ast::Klass>> classes;
    std::vector<std::reference_wrapper<ast::EnumNode>> enums;
    std::vector<std::reference_wrapper<ast::Function>> functions;

    Module(std::vector<std::unique_ptr<ast::Node>> all, std::vector<std::reference_wrapper<ast::Import>> imports,
           std::vector<std::reference_wrapper<ast::Klass>> classes,
           std::vector<std::reference_wrapper<ast::EnumNode>> enums,
           std::vector<std::reference_wrapper<ast::Function>> functions);
    std::vector<std::unique_ptr<ast::Node>> all;

    ~Module() {
        std::cout << "Calling ast::Module destructor" << std::endl;
    }
};


#endif //XLANG_AST_MODULE_H
