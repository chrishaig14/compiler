//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_TRANSLATOR_H
#define UNTITLED1_TRANSLATOR_H


#include "../nodes/Visitor.h"
#include "../nodes/nodes.h"
#include "../instructions/Instruction.h"
#include "../instructions/BinopInst.h"
#include "../instructions/CallInst.h"
#include "../instructions/DeclareInst.h"
#include "../instructions/PushStringInst.h"
#include "../instructions/ReturnInst.h"
#include "../instructions/SetMemberInst.h"
#include "../instructions/GetMemberInst.h"
#include "../instructions/SetInst.h"
#include "../instructions/GetInst.h"
#include "../instructions/PushIntegerInst.h"
#include "../instructions/SetSubscriptInst.h"
#include "../instructions/GetSubscriptInst.h"
#include "../instructions/PushFunctionInst.h"

class Translator : public Visitor {
public:
    void visit(AssignmentNode& node) override;

    void visit(BinopNode& node) override;

    void visit(BlockNode& node) override;

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

    Code code;
    bool is_lvalue;

    Translator();
};


#endif //UNTITLED1_TRANSLATOR_H
