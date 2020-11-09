//
// Created by chris on 8/11/20.
//

#ifndef NODECONTAINER_H
#define NODECONTAINER_H


class AssignmentNode;

class BinopNode;

class BlockNode;

class BooleanNode;

class BreakNode;

class CallNode;

class ClassLiteralExpressionNode;

class ClassLiteralFieldNode;

class ClassNode;

class ContinueNode;

class DeclarationNode;

class EmptyListNode;

class ForNode;

class FunctionNode;

class IdNode;

class IfNode;

class InstanceNode;

class ListNode;

class MemberNode;

class NoneNode;

class NumberNode;

class ReturnNode;

class StringNode;

class StructNode;

class SubscriptNode;

class TernaryNode;

class TypeNode;

class WhileNode;

class NodeContainer {
public:
    enum NodeType {
        ASSIGN,
        BINOP,
        BLOCK,
        BOOLEAN,
        BRK,
        CALL,
        CLSEXP,
        CLSFLD,
        CLS,
        CNTINUE,
        DECL,
        EMPTYLST,
        FORLOOP,
        FUNC,
        ID,
        IFN,
        INSTANCE,
        LST,
        MEMBER,
        NONE,
        NUMBER,
        RETRN,
        STRNG,
        STRCT,
        SUB,
        TERNARY,
        TYPE,
        WHIL,
        UNINITIALIZED
    } type;
    union {
        AssignmentNode* assign;
        BinopNode* binop;
        BlockNode* block;
        BooleanNode* boolean;
        BreakNode* brk;
        CallNode* call;
        ClassLiteralExpressionNode* clsexp;
        ClassLiteralFieldNode* clsfld;
        ClassNode* cls;
        ContinueNode* cntinue;
        DeclarationNode* decl;
        EmptyListNode* emptylst;
        ForNode* forloop;
        FunctionNode* func;
        IdNode* id;
        IfNode* ifn;
        InstanceNode* instance;
        ListNode* lst;
        MemberNode* member;
        NoneNode* none;
        NumberNode* number;
        ReturnNode* retrn;
        StringNode* strng;
        StructNode* strct;
        SubscriptNode* sub;
        TernaryNode* ternary;
        TypeNode* type;
        WhileNode* whil;
    } node;

    NodeContainer() : node({nullptr}) {
        this->type = UNINITIALIZED;
    }

    NodeContainer(const NodeContainer& other) {
        this->type = other.type;
        this->node = other.node;
    }

    NodeContainer(AssignmentNode* assign) {
        this->node.assign = assign;
        this->type = ASSIGN;
    }

    NodeContainer(BinopNode* binop) {
        this->node.binop = binop;
        this->type = BINOP;
    }

    NodeContainer(BlockNode* block) {
        this->node.block = block;
        this->type = BLOCK;
    }

    NodeContainer(BooleanNode* boolean) {
        this->node.boolean = boolean;
        this->type = BOOLEAN;
    }

    NodeContainer(BreakNode* brk) {
        this->node.brk = brk;
        this->type = BRK;
    }

    NodeContainer(CallNode* call) {
        this->node.call = call;
        this->type = CALL;
    }

    NodeContainer(ClassLiteralExpressionNode* clsexp) {
        this->node.clsexp = clsexp;
        this->type = CLSEXP;
    }

    NodeContainer(ClassLiteralFieldNode* clsfld) {
        this->node.clsfld = clsfld;
        this->type = CLSFLD;
    }

    NodeContainer(ClassNode* cls) {
        this->node.cls = cls;
        this->type = CLS;
    }

    NodeContainer(ContinueNode* cntinue) {
        this->node.cntinue = cntinue;
        this->type = CNTINUE;
    }

    NodeContainer(DeclarationNode* decl) {
        this->node.decl = decl;
        this->type = DECL;
    }

    NodeContainer(EmptyListNode* emptylst) {
        this->node.emptylst = emptylst;
        this->type = EMPTYLST;
    }

    NodeContainer(ForNode* forloop) {
        this->node.forloop = forloop;
        this->type = FORLOOP;
    }

    NodeContainer(FunctionNode* func) {
        this->node.func = func;
        this->type = FUNC;
    }

    NodeContainer(IdNode* id) {
        this->node.id = id;
        this->type = ID;
    }

    NodeContainer(IfNode* ifn) {
        this->node.ifn = ifn;
        this->type = IFN;
    }

    NodeContainer(InstanceNode* instance) {
        this->node.instance = instance;
        this->type = INSTANCE;
    }

    NodeContainer(ListNode* lst) {
        this->node.lst = lst;
        this->type = LST;
    }

    NodeContainer(MemberNode* member) {
        this->node.member = member;
        this->type = MEMBER;
    }

    NodeContainer(NoneNode* none) {
        this->node.none = none;
        this->type = NONE;
    }

    NodeContainer(NumberNode* number) {
        this->node.number = number;
        this->type = NUMBER;
    }

    NodeContainer(ReturnNode* retrn) {
        this->node.retrn = retrn;
        this->type = RETRN;
    }

    NodeContainer(StringNode* strng) {
        this->node.strng = strng;
        this->type = STRNG;
    }

    NodeContainer(StructNode* strct) {
        this->node.strct = strct;
        this->type = STRCT;
    }

    NodeContainer(SubscriptNode* sub) {
        this->node.sub = sub;
        this->type = SUB;
    }

    NodeContainer(TernaryNode* ternary) {
        this->node.ternary = ternary;
        this->type = TERNARY;
    }

    NodeContainer(TypeNode* type) {
        this->node.type = type;
        this->type = TYPE;
    }

    NodeContainer(WhileNode* whil) {
        this->node.whil = whil;
        this->type = WHIL;
    }

};

#endif //NODECONTAINER_H
