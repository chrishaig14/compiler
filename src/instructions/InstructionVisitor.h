//
// Created by chris on 3/8/20.
//

#ifndef INSTRUCTIONVISITOR_H
#define INSTRUCTIONVISITOR_H

class BinopInst;

class CallInst;


class DeclareInst;

class DefineStructInst;

class JumpInst;

class GetInst;

class GetMemberInst;

class GetSubscriptInst;

class JumpIfFalseInst;

class MakeClassInst;

class MakeListInst;

class MakeObjectInst;

class PushBooleanInst;

class PushFunctionInst;

class PushIntegerInst;

class PushStringInst;

class ReturnInst;

class SetInst;

class SetMemberInst;

class SetSubscriptInst;

class EnterScope;

class LeaveScope;

class MakeDefaultInst;

class NopInst;

class StartFunction;

class EndFunction;

class JumpIfNone;

class PushNone;

class PopInst;

class InstructionVisitor {
public:

    virtual void visit(CallInst& inst) = 0;

    virtual void visit(PopInst& inst) = 0;

    virtual void visit(PushNone& inst) = 0;

    virtual void visit(EndFunction& inst) = 0;

    virtual void visit(StartFunction& inst) = 0;

    virtual void visit(DeclareInst& inst) = 0;

    virtual void visit(DefineStructInst& inst) = 0;

    virtual void visit(GetInst& inst) = 0;

    virtual void visit(NopInst& inst) = 0;

    virtual void visit(GetMemberInst& inst) = 0;

    virtual void visit(LeaveScope& inst) = 0;

    virtual void visit(EnterScope& inst) = 0;

    virtual void visit(GetSubscriptInst& inst) = 0;

    virtual void visit(JumpIfFalseInst& inst) = 0;

    virtual void visit(JumpIfNone& inst) = 0;

    virtual void visit(JumpInst& inst) = 0;

    virtual void visit(MakeClassInst& inst) = 0;

    virtual void visit(MakeDefaultInst& inst) = 0;

    virtual void visit(MakeListInst& inst) = 0;

    virtual void visit(MakeObjectInst& inst) = 0;

    virtual void visit(PushBooleanInst& inst) = 0;

    virtual void visit(PushFunctionInst& inst) = 0;

    virtual void visit(PushIntegerInst& inst) = 0;

    virtual void visit(PushStringInst& inst) = 0;

    virtual void visit(ReturnInst& inst) = 0;

    virtual void visit(SetInst& inst) = 0;

    virtual void visit(SetMemberInst& inst) = 0;

    virtual void visit(SetSubscriptInst& inst) = 0;

    virtual void visit(BinopInst& inst) = 0;
};


#endif //INSTRUCTIONVISITOR_H
