//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <unordered_map>
#include "../units/infos/Module.h"
#include "errors/include/ErrorReporter.h"

class ModulePrechecker {
public:
    Module& module;
    ErrorReporter error_reporter;
    explicit ModulePrechecker(Module& module);

    void visit_root();
    void visit_function(ast::Function& node);
    void visit_enum(ast::EnumNode& node);
    void visit_alias(ast::Alias& node);
    void visit_import(ast::Import& node);
    void visit_class(ast::ConcreteClassDef& node);
    void visit_template_class(ast::TemplateClassDef& node);
    void check_duplicated_names(ast::Module& node);
    void add_default_imports();
    void visit_typeclass(ast::TypeclassAst& typeclass);
};

#endif //GLOBALPROCESSOR_H
