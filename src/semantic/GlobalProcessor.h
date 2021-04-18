//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <unordered_map>
#include "SymbolTable.h"
#include "../nodes/ClassNode.h"


typedef std::pair<std::string, std::string> Builtin;

class GlobalProcessor {
public:
    std::string module_dotted_path;
    std::map<std::string, VectorOfStrings> imported_paths;

    Package* root_package;
    Module* module;
    GlobalProcessor();
    void visit(FunctionNode& node);

    void dispatch(Node* nod);

    void visit(BlockNode& node);
    void visit_root(BlockNode& node);

    void visit(ImportNode& node);

    void visit(ClassNode& node);



    std::string __file__;
    std::string module_name;
    std::string get_actual_path(std::string id);
};

#endif //GLOBALPROCESSOR_H
