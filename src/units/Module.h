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
    Module(std::string name, std::string abs_path, std::string rel_path);
    std::string name;
    VectorOfStrings included_module_paths;
    std::string abs_path;
    std::string rel_path;
    BlockNode* ast;
    std::map<std::string, VectorOfStrings> imported_paths;
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
};


#endif //XLANG_MODULE_H
