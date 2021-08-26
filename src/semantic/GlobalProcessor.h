//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <unordered_map>
#include "SymbolTable.h"
#include "../ast/ImportNode.h"
#include "../ast/ClassNode.h"
#include "../ast/AliasNode.h"
#include "../ast/EnumNode.h"
#include "../units/Enum.h"

class GlobalProcessor {
public:
    Module& module;

    explicit GlobalProcessor(Module& module);

    void visit_root();

    void dispatch(Node& nod);
    void visit_function(FunctionNode& node);
    void visit_block(BlockNode& node);
    void visit_enum(EnumNode& node);
    void visit_alias(AliasNode& node);
    void visit_import(ImportNode& node);
    void visit_class(ast::ClassNode& node);
    void check_duplicated_names(BlockNode& node) const;
    void add_default_imports();
};

#endif //GLOBALPROCESSOR_H
