//
// Created by chris on 6/4/21.
//

#ifndef XLANG_MODULE_H
#define XLANG_MODULE_H

#include <string>
#include "Entity.h"
#include "../nodes/BlockNode.h"
#include "../scanner/CodeLines.h"
#include "../simple_nodes/BlockSNode.h"
#include "ConstFunction.h"
#include "Class.h"

class Module {
public:
    Module(Path path, std::string abs_path, std::string rel_path);
    std::string name;
    VectorOfStrings included_module_paths;
    std::string abs_path;
    std::string rel_path;
    BlockNode* ast;
    std::map<std::string, Path> imported_paths_no_alias;
    std::map<std::string, Path> imported_paths_with_alias;
    std::map<std::string, TypeNode*> aliased_types;
    std::vector<std::pair<std::string, Path>> imported_paths_no_alias_v;
    std::vector<std::pair<std::string, Path>> imported_paths_with_alias_v;
    std::map<std::string, Entity> imports;
    Path path;
    CodeLines code_lines;
    BlockSNode* sast;
    std::map<std::string, Class*> classes;
    std::map<std::string, ConstFunction*> functions;
    std::map<std::string, Flirpin> flirpins;

    Flirpin get(Path p) {
        VectorOfStrings pt = p.as_vec();
        return this->flirpins[pt.back()];
    }

    Path get_actual_path(const std::string& id);
    void fill_actual(TypeNode* t);
    void fill_actual(ObjectType* t);
    void fill_actual(FunctionType* t);
};


#endif //XLANG_MODULE_H
