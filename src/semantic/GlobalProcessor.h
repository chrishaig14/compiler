//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <unordered_map>
#include "SymbolTable.h"
#include "../ast/ImportNode.h"
#include "../ast/Klass.h"
#include "../ast/AliasNode.h"
#include "../ast/EnumNode.h"
#include "../units/Enum.h"

class GlobalProcessor {
public:
    Module& module;

    explicit GlobalProcessor(Module& module);

    void visit_root();

    void dispatch(ast::Node& nod);
    void visit_function(ast::Function& node);
    void visit_block(ast::Block& node);
    void visit_enum(ast::EnumNode& node);
    void visit_alias(ast::AliasNode& node);
    void visit_import(ast::ImportNode& node);
    void visit_class(ast::Klass& node);
    void check_duplicated_names(ast::Block& node) const;
    void add_default_imports();
};

#endif //GLOBALPROCESSOR_H
