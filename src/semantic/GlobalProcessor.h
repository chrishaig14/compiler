//
// Created by chris on 28/6/20.
//

#ifndef UNTITLED1_GLOBALPROCESSOR_H
#define UNTITLED1_GLOBALPROCESSOR_H


#include <map>
#include "../nodes/nodes.h"
#include "SymbolTable.h"
#include "ClassTable.h"

class GlobalProcessor : public Visitor{
public:
    SymbolTable* globals;
    ClassTable* class_table;

    GlobalProcessor();

    void visit(AssignmentNode& node) override;

    void visit(BinopNode& node) override;

    void visit(CallNode& node) override;

    void visit(ClassNode& node) override;

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
};

#endif //UNTITLED1_GLOBALPROCESSOR_H
