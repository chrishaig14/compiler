//
// Created by chris on 6/4/21.
//

#ifndef XLANG_MODULE_H
#define XLANG_MODULE_H

#include <string>
#include "../entities/Entity.h"
#include <scanner/CodeLines.h>
#include <simple_nodes/top/include/top.h>
#include <ast/top/Module.h>
#include <simple_nodes/common/include/Block.h>
#include "ConstFunction.h"
#include "ConcreteClass.h"
#include "TemplateClass.h"
#include "Enum.h"
#include "TypeclassFoo.h"

class Module {
    std::vector<std::unique_ptr<ConstFunction>> const_functions;
    std::vector<std::unique_ptr<Enum>> enums;
    std::vector<std::unique_ptr<ConcreteClass>> classes;
    std::vector<std::unique_ptr<TemplateClassInfo>> template_classes;
    std::vector<std::unique_ptr<TypeclassFoo>> typeclasses;
public:
    const std::string name;
    const std::string abs_path;

    const bool is_lib;
    const Path path;

    CodeLines code_lines;
    std::unique_ptr<ast::Module> ast;
    std::unique_ptr<sem::Module> sast;
    std::map<std::string, std::string> included_module_paths;
    std::map<std::string, Path> imported_paths_no_alias;
    std::map<std::string, Path> imported_paths_with_alias;
    std::map<std::string, ast::Type*> aliased_types;
    std::vector<std::pair<std::string, Path>> imported_paths_no_alias_v;
    std::vector<std::pair<std::string, Path>> imported_paths_with_alias_v;
    std::map<std::string, Entity*> imports;
    std::map<std::string, std::unique_ptr<ModuleMember>> members;

    ModuleMember* get(Path p);
    Module(Path path, std::string abs_path, bool is_lib);

    Path get_actual_path(const std::string& id);
    // void fill_actual(ast::Type& t);
    // void fill_actual(ast::ObjectType& t);
    // void fill_actual(ast::FunctionType& t);
    void fill_actual(sem::Type& t);
    void fill_actual(sem::TypeObject& t);
    void fill_actual(sem::TypeFunction& t);
    void add_class_definition(std::unique_ptr<ConcreteClass> p_class);
    void add_template_class_definition(std::unique_ptr<TemplateClassInfo> p_class);
    void add_enum_definition(std::unique_ptr<Enum> enumm);
    void add_func_definition(std::unique_ptr<ConstFunction> const_function);
    void add_typeclass_definition(std::unique_ptr<TypeclassFoo> typeclass);
};


#endif //XLANG_MODULE_H
