//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <map>
#include "SymbolTable.h"
#include "ClassTable.h"
#include "../vm/Object.h"
#include "../nodes/ClassNode.h"

class FunctionTable {
    std::map<std::string, FunctionTypeNode*> functions;

public:

    void add(std::string function_name, FunctionTypeNode& function_type);

    bool has_function(std::string name);

    const FunctionTypeNode& get(std::string function_name);

};

typedef std::pair<std::string, CodeBuiltin> Builtin;

class GlobalProcessor {
public:
    SymbolTable* globals;
    ClassTable* class_table;
    FunctionTable* function_table;

    GlobalProcessor();

    GlobalProcessor(std::vector<Builtin>& builtins);

    void add_builtins(std::vector<Builtin>& builtins);

    void visit(AssignmentNode& node);

    void visit(BinopNode& node);

    void visit(CallNode& node);

    void visit(DeclarationNode& node);

    void visit(FunctionNode& node);

    void visit(IdNode& node);

    void visit(IfNode& node);

    void visit(ListNode& node);

    void visit(MemberNode& node);

    void visit(NumberNode& node);

    void visit(ReturnNode& node);

    void dispatch(Node* nod);
    void visit(StringNode& node);

    void visit(SubscriptNode& node);

    void visit(VectorOfNodes program);

    void visit(BlockNode& node);

    void visit(ClassLiteralExpressionNode& node);

    void visit(ClassLiteralFieldNode& node);

    void visit(ForNode& node);

    void visit(BooleanNode& node);

    void visit(WhileNode& node);

    void visit(BreakNode& node);

    void visit(TernaryNode& node);

    void visit(NoneNode& node);

    void visit(EmptyListNode& node);

    void visit(ClassNode& node);

    void visit(InstanceNode& node);

    void visit(ContinueNode& node);
};

#endif //GLOBALPROCESSOR_H
