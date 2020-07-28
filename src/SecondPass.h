//
// Created by chris on 28/6/20.
//

#ifndef UNTITLED1_SECONDPASS_H
#define UNTITLED1_SECONDPASS_H

#include <map>
#include "FirstPass.h"

class BadArguments : public std::runtime_error {
public:
    BadArguments();

    bool operator==(const BadArguments& other) const;
};

class ReturnError : public std::runtime_error {
public:
    ReturnError(std::string actual_type, std::string expected_type);

    bool operator==(const ReturnError& other) const;
};

class ScopeError : public std::runtime_error {
public:
    ScopeError(std::string name);

    bool operator==(const ScopeError& other) const;
};

SymbolInfo* wrap_simple_info(ObjectInfo* sinfo);

SymbolInfo* wrap_function_info(FunctionInfo* finfo);

class RedeclareError : public std::runtime_error {
public:
    RedeclareError(std::string name);

    bool operator==(const RedeclareError& other) const;
};

ObjectInfo* s_info(TypeNode* type);

FunctionInfo* f_info(VectorOfTypes parameter_types, TypeNode* return_type);


SymbolInfo* w_sinfo(std::string type);

SymbolInfo* w_finfo(VectorOfTypes parameter_types, TypeNode* return_type);

typedef std::map<std::string, ObjectInfo*> MapStringToSimple;
typedef std::map<std::string, FunctionInfo*> MapStringToFunction;

SymbolInfo* w_cinfo(MapStringToSimple fields, MapStringToFunction methods);

struct SemanticInfo {
    SymbolInfo* symbol_info;
    std::map<std::string, int> declared_variables;
    std::map<std::string, int> free_variables;
};

class SecondPass {
    SymbolTable* scope;
    ClassTable* class_table;
public:
    SecondPass(SymbolTable* globals, ClassTable* class_table);

    void enter_scope(std::string name);

    void leave_scope();

    SemanticInfo* analyze(FunctionNode* n);

    SemanticInfo* analyze(IdentifierNode* n);

    SemanticInfo* analyze(DeclarationNode* n);

    SemanticInfo* analyze(MemberNode* n);

    SemanticInfo* analyze(IfNode* n);

    SemanticInfo* analyze(BinopNode* n);

    SemanticInfo* analyze(ReturnNode* n);

    SemanticInfo* analyze(ClassNode* n);

    SemanticInfo* analyze(AstNode* n);

    SemanticInfo* analyze(VectorOfNodes program);

    std::map<std::string, SymbolTable*> scopes;

    SemanticInfo* analyze(CallNode* n);

    SemanticInfo* analyze(AssignmentNode* n);
};

#endif //UNTITLED1_SECONDPASS_H
