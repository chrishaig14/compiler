//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <unordered_map>
#include "SymbolTable.h"
#include "ClassTable.h"
#include "../nodes/ClassNode.h"

class FunctionTable {
    std::unordered_map<std::string, FunctionType*> functions;

public:

    void add(std::string function_name, FunctionType* function_type);

    bool has_function(std::string name);

    ~FunctionTable();

    const FunctionType& get(std::string function_name);

};

typedef std::pair<std::string, std::string> Builtin;

class GlobalProcessor {
public:
    ClassTable* class_table;
    FunctionTable* function_table;

    GlobalProcessor(std::map<std::string, std::unique_ptr<std::map<std::string, std::string>>>& module_mappings,
                    ClassTable* imported_classes, FunctionTable* imported_functions, std::string module_name);

    void visit(FunctionNode& node);

    void dispatch(Node* nod);

    void visit(BlockNode& node);

    void visit(ImportNode& node);

    void visit(ClassNode& node);

    std::string __file__;
    std::map<std::string, std::unique_ptr<std::map<std::string, std::string>>>& module_mappings;
    std::string module_name;
};

#endif //GLOBALPROCESSOR_H
