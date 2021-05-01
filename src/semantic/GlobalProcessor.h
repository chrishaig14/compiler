//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <unordered_map>
#include "SymbolTable.h"
#include "../nodes/ClassNode.h"
#include "../nodes/AliasNode.h"
#include "../nodes/EnumNode.h"
#include "../units/Enum.h"


typedef std::pair<std::string, std::string> Builtin;

class GlobalProcessor {
public:
    Path module_dotted_path;

    Package* root_package;
    Module* module;
    GlobalProcessor();
    void visit(FunctionNode& node);

    void dispatch(Node* nod);

    void visit(BlockNode& node);
    void visit(EnumNode& node);
    void visit(AliasNode& node);
    void visit_root(BlockNode& node);

    void visit(ImportNode& node);

    void visit(ClassNode& node);


    std::string __file__;
    std::string module_name;

    Class* get_actual_clazz(std::string basicString);
};

#endif //GLOBALPROCESSOR_H
