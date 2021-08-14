//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <unordered_map>
#include "SymbolTable.h"
#include "../nodes/ImportNode.h"
#include "../nodes/ClassNode.h"
#include "../nodes/AliasNode.h"
#include "../nodes/EnumNode.h"
#include "../units/Enum.h"

class GlobalProcessor {
public:
    Module* module;

    GlobalProcessor(Module* module);

    void visit_root(BlockNode& node);

    void dispatch(Node* nod);
    void visit_function(FunctionNode& node);
    void visit_block(BlockNode& node);
    void visit_enum(EnumNode& node);
    void visit_alias(AliasNode& node);
    void visit_import(ImportNode& node);
    void visit_class(ClassNode& node);
    void check_duplicated_names(BlockNode& node) const;
    void add_default_imports();
};

#endif //GLOBALPROCESSOR_H
