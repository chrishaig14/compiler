//
// Created by chris on 2/8/20.
//

#ifndef TRANSLATOR_H
#define TRANSLATOR_H


#include "../nodes/nodes.h"
#include "../instructions/Instruction.h"
#include "../instructions/BinopInst.h"
#include "../instructions/PopInst.h"
#include "../instructions/CallInst.h"
#include "../instructions/JumpIfNone.h"
#include "../instructions/BoolOpInst.h"
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
#include "../instructions/MakeTupleInst.h"
#include "../instructions/PushIntegerInst.h"
#include "../instructions/SetSubscriptInst.h"
#include "../instructions/GetSubscriptInst.h"
#include "../instructions/GetTupleMemberInst.h"
#include "../instructions/EndFunction.h"
#include "../instructions/StartFunction.h"
#include "../instructions/JumpIfFalseInst.h"
#include "../instructions/PushFunctionInst.h"
#include "../instructions/PushBooleanInst.h"
#include "../instructions/JumpInst.h"
#include "../instructions/EnterScope.h"
#include "../instructions/PushFloatInst.h"
#include "../instructions/LeaveScope.h"

class Translator {
public:

    void dispatch(Node* nptr);

    int loop_counter;
    void visit(FloatNode& node);
    void visit(AssignmentNode& node);

    void visit(BinopNode& node);
    void visit(BoolOpNode& node);

    void visit(BlockNode& node);

    void visit(CallNode& node);

    void visit(DeclarationNode& node);

    void visit(FunctionNode& node);

    void visit(IdNode& node);

    void visit(IfNode& node);

    void visit(ListNode& node);

    void visit(MemberNode& node);

    void visit(NumberNode& node);

    void visit(ReturnNode& node);

    void visit(StringNode& node);

    void visit(SubscriptNode& node);

    CodeLabel code;
    bool is_lvalue;
    int current_loop;

    Translator();

    void visit(ClassLiteralExpressionNode& node);

    void visit(ClassLiteralFieldNode& node);

    void visit(ForNode& node);

    void visit(WhileNode& node);

    void visit(BooleanNode& node);

    void visit(BreakNode& node);

    void visit(TernaryNode& node);

    void visit(NoneNode& node);

    void visit(EmptyListNode& node);

    void visit(ClassNode& node);

    void visit(ContinueNode& node);

    bool in_for_loop;
    void visit(TupleNode& node);
};


#endif //TRANSLATOR_H
