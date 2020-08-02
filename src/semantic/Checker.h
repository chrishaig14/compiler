//
// Created by chris on 28/6/20.
//

#ifndef UNTITLED1_CHECKER_H
#define UNTITLED1_CHECKER_H

#include <map>
#include "GlobalProcessor.h"
#include "ScopeError.h"
#include "RedeclareError.h"
#include "ReturnError.h"
#include "BadArguments.h"

typedef std::map<std::string, ObjectInfo> MapStringToSimple;
typedef std::map<std::string, FunctionInfo> MapStringToFunction;


struct SemanticInfo {
    SymbolInfo* symbol_info;
    std::map<std::string, int> declared_variables;
    std::map<std::string, int> free_variables;
};

class Checker : public Visitor {
    SymbolTable* scope;
    ClassTable* class_table;
    SemanticInfo rv;
public:
    std::map<std::string, SymbolTable*> scopes;

    Checker(SymbolTable* globals, ClassTable* class_table);

    void enter_scope(std::string name);

    void leave_scope();

    void visit(AssignmentNode& node) override;

    void visit(BinopNode& node) override;

    void visit(CallNode& node) override;

    void visit(ClassNode& node) override;

    void visit(DeclarationNode& node) override;

    void visit(FunctionNode& node) override;

    void visit(IdNode& node) override;

    void visit(IfNode& node) override;

    void visit(ListNode& node) override {}

    void visit(MemberNode& node) override;

    void visit(NumberNode& node) override {
        SemanticInfo semanticInfo;
        semanticInfo.symbol_info = new SymbolInfo(new ObjectInfo(new TypeNode("Integer", {})));
        this->rv = semanticInfo;
    }

    void visit(ReturnNode& node) override;

    void visit(StringNode& node) override {
        SemanticInfo semanticInfo;
        semanticInfo.symbol_info = new SymbolInfo(new ObjectInfo(new TypeNode("String", {})));
        this->rv = semanticInfo;
    }

    void visit(SubscriptNode& node) override {

    }

    void visit(TypeNode& node) override {}

    void visit(BlockNode& node) override;


};

#endif //UNTITLED1_CHECKER_H
