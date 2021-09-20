//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <unordered_map>
#include "SymbolTable.h"
#include "../ast/Import.h"
#include "../ast/top/Klass.h"
#include "../ast/Alias.h"
#include "../ast/top/EnumNode.h"
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
    void visit_alias(ast::Alias& node);
    void visit_import(ast::Import& node);
    void visit_class(ast::Klass& node);
    void check_duplicated_names(ast::Module& node) const;
    void add_default_imports();
};

#endif //GLOBALPROCESSOR_H
