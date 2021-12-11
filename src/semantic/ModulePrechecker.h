//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <unordered_map>
#include <units/infos/Module.h>
#include "errors/include/ErrorReporter.h"

class ModulePrechecker {
public:
    Module& module;
    error::ErrorReporter error_reporter;
    std::vector<std::reference_wrapper<ConcreteClass>>& all_classes;
    std::map<std::string, std::set<std::string>>& instances;
    explicit ModulePrechecker(Module& module, std::map<std::string, std::set<std::string>>& instances,
                              std::vector<std::reference_wrapper<ConcreteClass>>& all_classes);

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
    void visit_instance(ast::Instance& instance);
    sem::Type* make_sem_type(const ast::Type& t);
};

#endif //GLOBALPROCESSOR_H
