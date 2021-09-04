//
// Created by chris on 6/4/21.
//

#ifndef XLANG_MODULE_H
#define XLANG_MODULE_H

#include <string>
#include "Entity.h"
#include "../ast/Block.h"
#include "../scanner/CodeLines.h"
#include "../simple_nodes/Block.h"
#include "ConstFunction.h"
#include "Class.h"

class Module {
public:
    std::string name;
    std::vector<std::unique_ptr<ConstFunction>> const_functions;
    std::string abs_path;
    CodeLines code_lines;
    ast::UBlock ast;
    std::unique_ptr<sem::Block> sast;

    std::map<std::string, std::string> included_module_paths;
    bool is_lib;
    std::map<std::string, Path> imported_paths_no_alias;
    std::map<std::string, Path> imported_paths_with_alias;
    std::map<std::string, TypeNode*> aliased_types;
    std::vector<std::pair<std::string, Path>> imported_paths_no_alias_v;
    std::vector<std::pair<std::string, Path>> imported_paths_with_alias_v;
    std::map<std::string, Entity*> imports;
    Path path;
    std::map<std::string, Flirpin> flirpins;
    std::string full_header_path;

    Flirpin get(Path p);
    Module(Path path, std::string abs_path, std::string rel_path, bool is_lib, std::string full_header_path);
    Path get_actual_path(const std::string& id);
    void fill_actual(TypeNode& t);
    void fill_actual(ObjectType& t);
    void fill_actual(FunctionType& t);
};


#endif //XLANG_MODULE_H
