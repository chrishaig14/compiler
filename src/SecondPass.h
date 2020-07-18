//
// Created by chris on 28/6/20.
//

#ifndef UNTITLED1_SECONDPASS_H
#define UNTITLED1_SECONDPASS_H

#include <map>
#include "FirstPass.h"

class ReturnError : public std::runtime_error {
public:
    ReturnError(std::string actual_type, std::string expected_type);

    bool operator==(const ReturnError &other) const;
};

class ScopeError : public std::runtime_error {
public:
    ScopeError(std::string name);

    bool operator==(const ScopeError &other) const;
};

SymbolInfo* wrap_simple_info(SimpleInfo* sinfo);

SymbolInfo* wrap_function_info(FunctionInfo* finfo);

class RedeclareError : public std::runtime_error {
public:
    RedeclareError(std::string name);

    bool operator==(const RedeclareError &other) const;
};

SimpleInfo* s_info(TypeNode* type);

FunctionInfo* f_info(VectorOfTypes parameter_types, TypeNode* return_type);


SymbolInfo* w_sinfo(std::string type);

SymbolInfo* w_finfo(VectorOfTypes parameter_types, TypeNode* return_type);
typedef std::map<std::string, SimpleInfo*> MapStringToSimple;
typedef std::map<std::string, FunctionInfo*> MapStringToFunction;

SymbolInfo* w_cinfo(MapStringToSimple fields, MapStringToFunction methods);


class SecondPass {
    SymbolTable* scope;
public:
    SecondPass(SymbolTable* globals);

    void enter_scope(std::string name);

    void leave_scope();

    SymbolInfo* analyze(FunctionNode* n);

    SymbolInfo* analyze(IdentifierNode* n);

    SymbolInfo* analyze(DeclarationNode* n);

    SymbolInfo* analyze(MemberNode* n);

    SymbolInfo* analyze(IfNode* n);

    SymbolInfo* analyze(BinopNode* n);

    SymbolInfo* analyze(ReturnNode* n);

    SymbolInfo* analyze(ClassNode* n);

    SymbolInfo* analyze(AstNode* n);

    SymbolInfo* analyze(VectorOfNodes program);

    std::map<std::string, SymbolTable*> scopes;

    SymbolInfo* analyze(CallNode* n);
};

#endif //UNTITLED1_SECONDPASS_H
