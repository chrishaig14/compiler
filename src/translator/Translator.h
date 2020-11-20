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

class Translator {
public:

    void dispatch(Node* nptr) {
        Node& n = *nptr;
        switch (n.ntype) {
            case NodeType::ASSIGN:
                this->visit(n.assign());
                break;
            case NodeType::BINOP:
                this->visit(n.binop());
                break;
            case NodeType::BLOCK:
                this->visit(n.block());
                break;
            case NodeType::BOOLEAN:
                this->visit(n.boolean());
                break;
            case NodeType::BRK:
                this->visit(n.brk());
                break;
            case NodeType::CALL:
                this->visit(n.call());
                break;
            case NodeType::CLSEXP:
                this->visit(n.clsexp());
                break;
            case NodeType::CLSFLD:
                this->visit(n.clsfld());
                break;
            case NodeType::CLS:
                this->visit(n.cls());
                break;
            case NodeType::CNTINUE:
                this->visit(n.cntinue());
                break;
            case NodeType::DECL:
                this->visit(n.decl());
                break;
            case NodeType::EMPTYLST:
                this->visit(n.emptylst());
                break;
            case NodeType::FORLOOP:
                this->visit(n.forloop());
                break;
            case NodeType::FUNC:
                this->visit(n.func());
                break;
            case NodeType::ID:
                this->visit(n.id());
                break;
            case NodeType::IFF:
                this->visit(n.iff());
                break;
            case NodeType::LST:
                this->visit(n.lst());
                break;
            case NodeType::MEMBER:
                this->visit(n.member());
                break;
            case NodeType::NONE:
                this->visit(n.none());
                break;
            case NodeType::NUMBER:
                this->visit(n.number());
                break;
            case NodeType::RETRN:
                this->visit(n.retrn());
                break;
            case NodeType::STRNG:
                this->visit(n.strng());
                break;
            case NodeType::SUB:
                this->visit(n.sub());
                break;
            case NodeType::TERNARY:
                this->visit(n.ternary());
                break;
            case NodeType::OTYPE:
//                this->visit(n.type());
                break;
            case NodeType::FTYPE:
//                this->visit(n.type());
                break;
            case NodeType::WHIL:
                this->visit(n.whil());
                break;
            case NodeType::UNINITIALIZED:
                break;
        }
    }

    int loop_counter;

    void visit(AssignmentNode& node);

    void visit(BinopNode& node);

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
};


#endif //TRANSLATOR_H
