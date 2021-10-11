//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <unordered_map>
#include "SymbolTable.h"
#include "../ast/top/Import.h"
#include "../ast/top/Klass.h"
#include "../ast/top/Alias.h"
#include "../ast/top/EnumNode.h"
#include "../units/infos/Enum.h"
#include "../units/entities/EntityNothing.h"
#include "../units/entities/EntityPackage.h"
#include "../units/infos/Module.h"
#include "errors/include/ErrorReporter.h"

class ModulePrechecker {
public:
    Module& module;
    ErrorReporter error_reporter;
    explicit ModulePrechecker(Module& module);

    void visit_root();

    void dispatch(ast::Statement& nod);
    void visit_function(ast::Function& node);
    void visit_block(ast::Block& node);
    void visit_enum(ast::EnumNode& node);
    void visit_alias(ast::Alias& node);
    void visit_import(ast::Import& node);
    void visit_class(ast::Klass& node);
    void check_duplicated_names(ast::Module& node);
    void add_default_imports();
};

#endif //GLOBALPROCESSOR_H
