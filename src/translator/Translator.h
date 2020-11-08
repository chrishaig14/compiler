//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_TRANSLATOR_H
#define UNTITLED1_TRANSLATOR_H


#include "../nodes/Visitor.h"
#include "../nodes/nodes.h"
#include "../instructions/Instruction.h"
#include "../instructions/BinopInst.h"
#include "../instructions/PopInst.h"
#include "../instructions/CallInst.h"
#include "../instructions/JumpIfNone.h"
#include "../instructions/DeclareInst.h"
#include "../instructions/PushStringInst.h"
#include "../instructions/ReturnInst.h"
#include "../instructions/MakeObjectInst.h"
#include "../instructions/MakeListInst.h"
#include "../instructions/SetMemberInst.h"
#include "../instructions/PushNone.h"
#include "../instructions/MakeClassInst.h"
#include "../instructions/GetMemberInst.h"
#include "../instructions/SetInst.h"
#include "../instructions/GetInst.h"
#include "../instructions/PushIntegerInst.h"
#include "../instructions/SetSubscriptInst.h"
#include "../instructions/GetSubscriptInst.h"
#include "../instructions/EndFunction.h"
#include "../instructions/StartFunction.h"
#include "../instructions/JumpIfFalseInst.h"
#include "../instructions/PushFunctionInst.h"
#include "../instructions/PushBooleanInst.h"
#include "../instructions/JumpInst.h"
#include "../instructions/EnterScope.h"
#include "../instructions/LeaveScope.h"

class Translator : public Visitor {
public:
    int loop_counter;

    void visit(AssignmentNode& node) override;

    void visit(BinopNode& node) override;

    void visit(BlockNode& node) override;

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

    CodeLabel code;
    bool is_lvalue;
    int current_loop;

    Translator();

    void visit(ClassLiteralExpressionNode& node) override;

    void visit(ClassLiteralFieldNode& node) override;

    void visit(ForNode& node) override;

    void visit(WhileNode& node) override;

    void visit(BooleanNode& node) override;

    void visit(BreakNode& node) override;

    void visit(TernaryNode& node) override;

    void visit(NoneNode& node) override;

    void visit(EmptyListNode& node) override;

    void visit(ClassNode& node) override;

    void visit(InstanceNode& node) override;

    void visit(ContinueNode& node) override;

    bool in_for_loop;
};


#endif //UNTITLED1_TRANSLATOR_H
