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

class GlobalProcessor {
public:
    std::string __file__;
    std::string module_name;
    Module* module;

    GlobalProcessor();

    void visit_root(BlockNode& node);

    void dispatch(Node* nod);
    void visit_function(FunctionNode& node);
    void visit_block(BlockNode& node);
    void visit_enum(EnumNode& node);
    void visit_alias(AliasNode& node);
    void visit_import(ImportNode& node);
    void visit_class(ClassNode& node);
};

#endif //GLOBALPROCESSOR_H
