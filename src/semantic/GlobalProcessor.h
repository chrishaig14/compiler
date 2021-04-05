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
    ClassTable* global_classes;
    FunctionTable* global_functions;
    std::string module_dotted_path;
    std::map<std::string, std::string> imported_paths;
    std::map<std::string, std::string> local_paths;
    std::map<std::string, std::string>& global_path_to_mangled_name;

    GlobalProcessor(ClassTable* global_classes, FunctionTable* global_functions, std::string module_name,
                    std::map<std::string, std::string>& global_path_to_mangled_name);

    void visit(FunctionNode& node);

    void dispatch(Node* nod);

    void visit(BlockNode& node);

    void visit(ImportNode& node);

    void visit(ClassNode& node);

    std::string __file__;
    std::string module_name;
};

#endif //GLOBALPROCESSOR_H
