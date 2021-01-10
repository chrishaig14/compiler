//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <unordered_map>
#include "SymbolTable.h"
#include "ClassTable.h"
#include "../vm/Object.h"
#include "../nodes/ClassNode.h"

class FunctionTable {
    std::unordered_map<std::string, FunctionTypeNode*> functions;

public:

    void add(std::string function_name, const FunctionTypeNode& function_type);

    bool has_function(std::string name);

    ~FunctionTable() {
        for (auto f: this->functions) {
            delete f.second;
        }
    }

    const FunctionTypeNode& get(std::string function_name);

};

typedef std::pair<std::string, CodeBuiltin> Builtin;

class GlobalProcessor {
public:
    SymbolTable* globals;
    ClassTable* class_table;
    FunctionTable* function_table;

    GlobalProcessor();
    GlobalProcessor(std::vector<Builtin>& builtins, ClassTable* imported_classes,
                                     FunctionTable* imported_functions);

    void add_builtins(std::vector<Builtin>& builtins);

    void visit(FunctionNode& node);

    void dispatch(Node* nod);

    void visit(BlockNode& node);

    void visit(ClassNode& node);

    std::string __file__;
};

#endif //GLOBALPROCESSOR_H
