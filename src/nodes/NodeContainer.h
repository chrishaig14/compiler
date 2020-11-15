//
// Created by chris on 8/11/20.
//

#ifndef NODECONTAINER_H
#define NODECONTAINER_H

#include <stdexcept>

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
    IFF,
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
};
class NodeContainer {
private:


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
public:
    AssignmentNode& assign() const{
        if (this->ntype == ASSIGN) {
            return *this->node.assign;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: assign");
    };

    BinopNode& binop()const {
        if (this->ntype == BINOP) {
            return *this->node.binop;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: binop");
    };

    BlockNode& block() const{
        if (this->ntype == BLOCK) {
            return *this->node.block;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: block");
    };

    BooleanNode& boolean() const{
        if (this->ntype == BOOLEAN) {
            return *this->node.boolean;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: boolean");
    };

    BreakNode& brk()const {
        if (this->ntype == BRK) {
            return *this->node.brk;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: brk");
    };

    CallNode& call()const {
        if (this->ntype == CALL) {
            return *this->node.call;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: call");
    };

    ClassLiteralExpressionNode& clsexp()const {
        if (this->ntype == CLSEXP) {
            return *this->node.clsexp;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: clsexp");
    };

    ClassLiteralFieldNode& clsfld()const {
        if (this->ntype == CLSFLD) {
            return *this->node.clsfld;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: clsfld");
    };

    ClassNode& cls() const{
        if (this->ntype == CLS) {
            return *this->node.cls;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: cls");
    };

    ContinueNode& cntinue()const {
        if (this->ntype == CNTINUE) {
            return *this->node.cntinue;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: cntinue");
    };

    DeclarationNode& decl() const{
        if (this->ntype == DECL) {
            return *this->node.decl;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: decl");
    };

    EmptyListNode& emptylst() const{
        if (this->ntype == EMPTYLST) {
            return *this->node.emptylst;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: emptylst");
    };

    ForNode& forloop()const {
        if (this->ntype == FORLOOP) {
            return *this->node.forloop;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: forloop");
    };

    FunctionNode& func() const{
        if (this->ntype == FUNC) {
            return *this->node.func;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: func");
    };

    IdNode& id() const {
        if (this->ntype == ID) {
            return *this->node.id;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: id");
    };

    IfNode& iff() const {
        if (this->ntype == IFF) {
            return *this->node.iff;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: iff");
    };

    InstanceNode& instance() const {
        if (this->ntype == INSTANCE) {
            return *this->node.instance;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: instance");
    };

    ListNode& lst() const {
        if (this->ntype == LST) {
            return *this->node.lst;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: lst");
    };

    MemberNode& member() const {
        if (this->ntype == MEMBER) {
            return *this->node.member;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: member");
    };

    NoneNode& none() const {
        if (this->ntype == NONE) {
            return *this->node.none;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: none");
    };

    NumberNode& number() const {
        if (this->ntype == NUMBER) {
            return *this->node.number;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: number");
    };

    ObjectTypeNode& otype() const {
        if (this->ntype == OTYPE) {
            return *this->node.otype;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: otype");
    };

    FunctionTypeNode& ftype() const {
        if (this->ntype == FTYPE) {
            return *this->node.ftype;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: ftype");
    };

    ReturnNode& retrn() const {
        if (this->ntype == RETRN) {
            return *this->node.retrn;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: retrn");
    };

    StringNode& strng() const {
        if (this->ntype == STRNG) {
            return *this->node.strng;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: strng");
    };

    StructNode& strct() const {
        if (this->ntype == STRCT) {
            return *this->node.strct;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: strct");
    };

    SubscriptNode& sub() const {
        if (this->ntype == SUB) {
            return *this->node.sub;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: sub");
    };

    TernaryNode& ternary() const {
        if (this->ntype == TERNARY) {
            return *this->node.ternary;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: ternary");
    };

//    TypeNode& type() {
//        if (this->ntype == TYPE) {
//            return *this->node.type;
//        }
//        throw std::runtime_error("Getting wrong node out of NodeContainer: type");
//    };

    WhileNode& whil() const {
        if (this->ntype == WHIL) {
            return *this->node.whil;
        }
        throw std::runtime_error("Getting wrong node out of NodeContainer: whil");
    };
    NodeType ntype;


    NodeContainer() : node({nullptr}) {
        this->ntype = UNINITIALIZED;
    }

    NodeContainer(const NodeContainer& other) {
        this->ntype = other.ntype;
        this->node = other.node;
    }

    explicit NodeContainer(AssignmentNode* assign) {
        this->node.assign = assign;
        this->ntype = ASSIGN;
    }

    explicit NodeContainer(BinopNode* binop) {
        this->node.binop = binop;
        this->ntype = BINOP;
    }

    explicit NodeContainer(BlockNode* block) {
        this->node.block = block;
        this->ntype = BLOCK;
    }

    explicit NodeContainer(BooleanNode* boolean) {
        this->node.boolean = boolean;
        this->ntype = BOOLEAN;
    }

    explicit NodeContainer(BreakNode* brk) {
        this->node.brk = brk;
        this->ntype = BRK;
    }

    explicit NodeContainer(CallNode* call) {
        this->node.call = call;
        this->ntype = CALL;
    }

    explicit NodeContainer(ClassLiteralExpressionNode* clsexp) {
        this->node.clsexp = clsexp;
        this->ntype = CLSEXP;
    }

    explicit NodeContainer(ClassLiteralFieldNode* clsfld) {
        this->node.clsfld = clsfld;
        this->ntype = CLSFLD;
    }

    explicit NodeContainer(ClassNode* cls) {
        this->node.cls = cls;
        this->ntype = CLS;
    }

    explicit NodeContainer(ContinueNode* cntinue) {
        this->node.cntinue = cntinue;
        this->ntype = CNTINUE;
    }

    explicit NodeContainer(DeclarationNode* decl) {
        this->node.decl = decl;
        this->ntype = DECL;
    }

    explicit NodeContainer(EmptyListNode* emptylst) {
        this->node.emptylst = emptylst;
        this->ntype = EMPTYLST;
    }

    explicit NodeContainer(ForNode* forloop) {
        this->node.forloop = forloop;
        this->ntype = FORLOOP;
    }

    explicit NodeContainer(FunctionNode* func) {
        this->node.func = func;
        this->ntype = FUNC;
    }

    explicit NodeContainer(IdNode* id) {
        this->node.id = id;
        this->ntype = ID;
    }

    explicit NodeContainer(IfNode* ifn) {
        this->node.iff = ifn;
        this->ntype = IFF;
    }

    explicit NodeContainer(InstanceNode* instance) {
        this->node.instance = instance;
        this->ntype = INSTANCE;
    }

    explicit NodeContainer(ListNode* lst) {
        this->node.lst = lst;
        this->ntype = LST;
    }

    explicit NodeContainer(MemberNode* member) {
        this->node.member = member;
        this->ntype = MEMBER;
    }

    explicit NodeContainer(NoneNode* none) {
        this->node.none = none;
        this->ntype = NONE;
    }

    explicit NodeContainer(NumberNode* number) {
        this->node.number = number;
        this->ntype = NUMBER;
    }

    explicit NodeContainer(ReturnNode* retrn) {
        this->node.retrn = retrn;
        this->ntype = RETRN;
    }

    explicit NodeContainer(StringNode* strng) {
        this->node.strng = strng;
        this->ntype = STRNG;
    }

    explicit NodeContainer(StructNode* strct) {
        this->node.strct = strct;
        this->ntype = STRCT;
    }

    explicit NodeContainer(SubscriptNode* sub) {
        this->node.sub = sub;
        this->ntype = SUB;
    }

    explicit NodeContainer(TernaryNode* ternary) {
        this->node.ternary = ternary;
        this->ntype = TERNARY;
    }

    explicit NodeContainer(ObjectTypeNode* type) {
        this->node.otype = type;
        this->ntype = OTYPE;
    }

    explicit NodeContainer(FunctionTypeNode* type) {
        this->node.ftype = type;
        this->ntype = FTYPE;
    }

    explicit NodeContainer(WhileNode* whil) {
        this->node.whil = whil;
        this->ntype = WHIL;
    }

};

#endif //NODECONTAINER_H
