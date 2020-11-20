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

    void dispatch(Node* nptr) {
        Node& n = *nptr;
        switch (n.ntype) {
            case NodeType::ASSIGN:
                n.assign().accept(*this);
                break;
            case NodeType::BINOP:
                n.binop().accept(*this);
                break;
            case NodeType::BLOCK:
                n.block().accept(*this);
                break;
            case NodeType::BOOLEAN:
                n.boolean().accept(*this);
                break;
            case NodeType::BRK:
                n.brk().accept(*this);
                break;
            case NodeType::CALL:
                n.call().accept(*this);
                break;
            case NodeType::CLSEXP:
                n.clsexp().accept(*this);
                break;
            case NodeType::CLSFLD:
                n.clsfld().accept(*this);
                break;
            case NodeType::CLS:
                n.cls().accept(*this);
                break;
            case NodeType::CNTINUE:
                n.cntinue().accept(*this);
                break;
            case NodeType::DECL:
                n.decl().accept(*this);
                break;
            case NodeType::EMPTYLST:
                n.emptylst().accept(*this);
                break;
            case NodeType::FORLOOP:
                n.forloop().accept(*this);
                break;
            case NodeType::FUNC:
                n.func().accept(*this);
                break;
            case NodeType::ID:
                n.id().accept(*this);
                break;
            case NodeType::IFF:
                n.iff().accept(*this);
                break;
            case NodeType::INSTANCE:
//                n.instance().accept(*this);
                break;
            case NodeType::LST:
                n.lst().accept(*this);
                break;
            case NodeType::MEMBER:
                n.member().accept(*this);
                break;
            case NodeType::NONE:
                n.none().accept(*this);
                break;
            case NodeType::NUMBER:
                n.number().accept(*this);
                break;
            case NodeType::RETRN:
                n.retrn().accept(*this);
                break;
            case NodeType::STRNG:
                n.strng().accept(*this);
                break;
            case NodeType::SUB:
                n.sub().accept(*this);
                break;
            case NodeType::TERNARY:
                n.ternary().accept(*this);
                break;
            case NodeType::OTYPE:
//                n.type().accept(*this);
                break;
            case NodeType::FTYPE:
//                n.type().accept(*this);
                break;
            case NodeType::WHIL:
                n.whil().accept(*this);
                break;
            case NodeType::UNINITIALIZED:
                break;
        }
    }

    int loop_counter;

    void visit(AssignmentNode& node) override;

    void visit(BinopNode& node) override;

    void visit(BlockNode& node) override;

    void visit(CallNode& node) override;

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
