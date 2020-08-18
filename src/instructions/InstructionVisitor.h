//
// Created by chris on 3/8/20.
//

#ifndef UNTITLED1_INSTRUCTIONVISITOR_H
#define UNTITLED1_INSTRUCTIONVISITOR_H

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

class MakeDefaultInst;

class InstructionVisitor {
public:

    virtual void visit(CallInst& inst) = 0;

    virtual void visit(DeclareInst& inst) = 0;

    virtual void visit(DefineStructInst& inst) = 0;

    virtual void visit(GetInst& inst) = 0;

    virtual void visit(GetMemberInst& inst) = 0;

    virtual void visit(GetSubscriptInst& inst) = 0;

    virtual void visit(JumpIfFalseInst& inst) = 0;

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


#endif //UNTITLED1_INSTRUCTIONVISITOR_H
