//
// Created by chris on 2/8/20.
//

#ifndef TRANSLATOR_H
#define TRANSLATOR_H


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
    void dispatch(NodeContainer n) {
        switch (n.ntype) {
            case NodeContainer::ASSIGN:
                n.node.assign->accept(*this);
                break;
            case NodeContainer::BINOP:
                n.node.binop->accept(*this);
                break;
            case NodeContainer::BLOCK:
                n.node.block->accept(*this);
                break;
            case NodeContainer::BOOLEAN:
                n.node.boolean->accept(*this);
                break;
            case NodeContainer::BRK:
                n.node.brk->accept(*this);
                break;
            case NodeContainer::CALL:
                n.node.call->accept(*this);
                break;
            case NodeContainer::CLSEXP:
                n.node.clsexp->accept(*this);
                break;
            case NodeContainer::CLSFLD:
                n.node.clsfld->accept(*this);
                break;
            case NodeContainer::CLS:
                n.node.cls->accept(*this);
                break;
            case NodeContainer::CNTINUE:
                n.node.cntinue->accept(*this);
                break;
            case NodeContainer::DECL:
                n.node.decl->accept(*this);
                break;
            case NodeContainer::EMPTYLST:
                n.node.emptylst->accept(*this);
                break;
            case NodeContainer::FORLOOP:
                n.node.forloop->accept(*this);
                break;
            case NodeContainer::FUNC:
                n.node.func->accept(*this);
                break;
            case NodeContainer::ID:
                n.node.id->accept(*this);
                break;
            case NodeContainer::IFN:
                n.node.iff->accept(*this);
                break;
            case NodeContainer::INSTANCE:
//                n.node.instance->accept(*this);
                break;
            case NodeContainer::LST:
                n.node.lst->accept(*this);
                break;
            case NodeContainer::MEMBER:
                n.node.member->accept(*this);
                break;
            case NodeContainer::NONE:
                n.node.none->accept(*this);
                break;
            case NodeContainer::NUMBER:
                n.node.number->accept(*this);
                break;
            case NodeContainer::RETRN:
                n.node.retrn->accept(*this);
                break;
            case NodeContainer::STRNG:
                n.node.strng->accept(*this);
                break;
            case NodeContainer::STRCT:
                n.node.strct->accept(*this);
                break;
            case NodeContainer::SUB:
                n.node.sub->accept(*this);
                break;
            case NodeContainer::TERNARY:
                n.node.ternary->accept(*this);
                break;
            case NodeContainer::OTYPE:
//                n.node.type->accept(*this);
                break;
            case NodeContainer::FTYPE:
//                n.node.type->accept(*this);
                break;
            case NodeContainer::WHIL:
                n.node.whil->accept(*this);
                break;
            case NodeContainer::UNINITIALIZED:
                break;
        }
    }

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


#endif //TRANSLATOR_H
