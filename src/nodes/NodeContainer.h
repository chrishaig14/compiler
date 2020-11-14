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

class FunctionTypeNode;

class IdNode;

class IfNode;

class InstanceNode;

class ListNode;

class MemberNode;

class NoneNode;

class NumberNode;

class ObjectTypeNode;

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
        OTYPE,
        FTYPE,
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
        IfNode* iff;
        InstanceNode* instance;
        ListNode* lst;
        MemberNode* member;
        NoneNode* none;
        NumberNode* number;
        ObjectTypeNode* otype;
        FunctionTypeNode* ftype;
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

    explicit NodeContainer(AssignmentNode* assign) {
        this->node.assign = assign;
        this->type = ASSIGN;
    }

    explicit NodeContainer(BinopNode* binop) {
        this->node.binop = binop;
        this->type = BINOP;
    }

    explicit NodeContainer(BlockNode* block) {
        this->node.block = block;
        this->type = BLOCK;
    }

    explicit NodeContainer(BooleanNode* boolean) {
        this->node.boolean = boolean;
        this->type = BOOLEAN;
    }

    explicit NodeContainer(BreakNode* brk) {
        this->node.brk = brk;
        this->type = BRK;
    }

    explicit NodeContainer(CallNode* call) {
        this->node.call = call;
        this->type = CALL;
    }

    explicit NodeContainer(ClassLiteralExpressionNode* clsexp) {
        this->node.clsexp = clsexp;
        this->type = CLSEXP;
    }

    explicit NodeContainer(ClassLiteralFieldNode* clsfld) {
        this->node.clsfld = clsfld;
        this->type = CLSFLD;
    }

    explicit NodeContainer(ClassNode* cls) {
        this->node.cls = cls;
        this->type = CLS;
    }

    explicit NodeContainer(ContinueNode* cntinue) {
        this->node.cntinue = cntinue;
        this->type = CNTINUE;
    }

    explicit NodeContainer(DeclarationNode* decl) {
        this->node.decl = decl;
        this->type = DECL;
    }

    explicit NodeContainer(EmptyListNode* emptylst) {
        this->node.emptylst = emptylst;
        this->type = EMPTYLST;
    }

    explicit NodeContainer(ForNode* forloop) {
        this->node.forloop = forloop;
        this->type = FORLOOP;
    }

    explicit NodeContainer(FunctionNode* func) {
        this->node.func = func;
        this->type = FUNC;
    }

    explicit NodeContainer(IdNode* id) {
        this->node.id = id;
        this->type = ID;
    }

    explicit NodeContainer(IfNode* ifn) {
        this->node.iff = ifn;
        this->type = IFN;
    }

    explicit NodeContainer(InstanceNode* instance) {
        this->node.instance = instance;
        this->type = INSTANCE;
    }

    explicit NodeContainer(ListNode* lst) {
        this->node.lst = lst;
        this->type = LST;
    }

    explicit NodeContainer(MemberNode* member) {
        this->node.member = member;
        this->type = MEMBER;
    }

    explicit NodeContainer(NoneNode* none) {
        this->node.none = none;
        this->type = NONE;
    }

    explicit NodeContainer(NumberNode* number) {
        this->node.number = number;
        this->type = NUMBER;
    }

    explicit NodeContainer(ReturnNode* retrn) {
        this->node.retrn = retrn;
        this->type = RETRN;
    }

    explicit NodeContainer(StringNode* strng) {
        this->node.strng = strng;
        this->type = STRNG;
    }

    explicit NodeContainer(StructNode* strct) {
        this->node.strct = strct;
        this->type = STRCT;
    }

    explicit NodeContainer(SubscriptNode* sub) {
        this->node.sub = sub;
        this->type = SUB;
    }

    explicit NodeContainer(TernaryNode* ternary) {
        this->node.ternary = ternary;
        this->type = TERNARY;
    }

    explicit NodeContainer(ObjectTypeNode* type) {
        this->node.otype = type;
        this->type = OTYPE;
    }

    explicit NodeContainer(FunctionTypeNode* type) {
        this->node.ftype = type;
        this->type = FTYPE;
    }

    explicit NodeContainer(WhileNode* whil) {
        this->node.whil = whil;
        this->type = WHIL;
    }

};

#endif //NODECONTAINER_H
