//
// Created by chris on 28/6/20.
//

#ifndef GLOBALPROCESSOR_H
#define GLOBALPROCESSOR_H


#include <map>
#include "../nodes/nodes.h"
#include "SymbolTable.h"
#include "ClassTable.h"
#include "../vm/Object.h"

class FunctionTable {
    std::map<std::string, FunctionTypeNode> functions;

public:

    void add(std::string function_name, FunctionTypeNode& function_type);

    bool has_function(std::string name);

    FunctionTypeNode& get(std::string function_name);

};

typedef std::pair<std::string, CodeBuiltin> Builtin;

class GlobalProcessor : public Visitor {
public:
    SymbolTable* globals;
    ClassTable* class_table;
    FunctionTable* function_table;

    GlobalProcessor();

    GlobalProcessor(std::vector<Builtin>& builtins);

    void add_builtins(std::vector<Builtin>& builtins);

    void visit(AssignmentNode& node) override;

    void visit(BinopNode& node) override;

    void visit(CallNode& node) override;

    void visit(StructNode& node) override;

    void visit(DeclarationNode& node) override;

    void visit(FunctionNode& node) override;

    void visit(IdNode& node) override;

    void visit(IfNode& node) override;

    void visit(ListNode& node) override;

    void visit(MemberNode& node) override;

    void visit(NumberNode& node) override;

    void visit(ReturnNode& node) override;

    void visit(StringNode& node) override;

    void visit(SubscriptNode& node) override;

    void visit(TypeNode& node) override;

    void visit(VectorOfNodes program);

    void visit(BlockNode& node) override;

    void visit(ClassLiteralExpressionNode& node) override;

    void visit(ClassLiteralFieldNode& node) override;

    void visit(ForNode& node) override;

    void visit(BooleanNode& node) override;

    void visit(WhileNode& node) override;

    void visit(BreakNode& node) override;

    void visit(TernaryNode& node) override;

    void visit(NoneNode& node) override;

    void visit(EmptyListNode& node) override;

    void visit(ClassNode& node) override;

    void visit(InstanceNode& node) override;

    void visit(ContinueNode& node) override;

    void dispatch(NodeContainer container);
};

#endif //GLOBALPROCESSOR_H
