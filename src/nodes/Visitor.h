//
// Created by chris on 1/8/20.
//

#ifndef UNTITLED1_VISITOR_H
#define UNTITLED1_VISITOR_H

class AssignmentNode;

class BinopNode;

class BlockNode;

class CallNode;

class ClassLiteralExpressionNode;

class ClassLiteralFieldNode;

class StructNode;

class DeclarationNode;

class ForNode;

class FunctionNode;

class IdNode;

class IfNode;

class ListNode;

class BreakNode;

class MemberNode;

class NumberNode;

class ReturnNode;

class BooleanNode;

class StringNode;

class SubscriptNode;

class TypeNode;

class WhileNode;

class Visitor {
public:
    virtual void visit(AssignmentNode& node) = 0;

    virtual void visit(BinopNode& node) = 0;

    virtual void visit(BreakNode& node) = 0;

    virtual void visit(BlockNode& node) = 0;

    virtual void visit(CallNode& node) = 0;

    virtual void visit(ClassLiteralExpressionNode& node) = 0;

    virtual void visit(ClassLiteralFieldNode& node) = 0;

    virtual void visit(DeclarationNode& node) = 0;

    virtual void visit(ForNode& node) = 0;

    virtual void visit(FunctionNode& node) = 0;

    virtual void visit(BooleanNode& node) = 0;

    virtual void visit(IdNode& node) = 0;

    virtual void visit(IfNode& node) = 0;

    virtual void visit(ListNode& node) = 0;

    virtual void visit(MemberNode& node) = 0;

    virtual void visit(NumberNode& node) = 0;

    virtual void visit(ReturnNode& node) = 0;

    virtual void visit(StringNode& node) = 0;

    virtual void visit(StructNode& node) = 0;

    virtual void visit(SubscriptNode& node) = 0;

    virtual void visit(TypeNode& node) = 0;

    virtual void visit(WhileNode& node) = 0;
};


#endif //UNTITLED1_VISITOR_H
