//
// Created by chris on 17/11/20.
//

#ifndef UNTITLED1_NODE_TYPES_H
#define UNTITLED1_NODE_TYPES_H

enum class StatementType {
    ASSIGN,
    BINOP,
    BLOCK,
    PARTIAL,
    BOOLEAN,
    BRK,
    DEF_CONST,
    CALL,
    METHOD,
    CAST,
    DICT,
    EMPTYDICT,
    IMPORT,
    CLS,
    CNTINUE,
    DECL,
    EMPTYLST,
    FORLOOP,
    FUNC,
    ID,
    IFF,
    LST,
    MEMBER,
    NONE,
    NUMBER,
    RETRN,
    STRNG,
    SUB,
    TERNARY,
    TUPLE,
    OTYPE,
    FTYPE,
    WHIL,
    BOOLOP,
    MATCH_EXP,
    ALIAS,
    ENUM,
    UNARY,
    THROW,
    TRY_CATCH,
    TYPECLASS,
    INSTANCE,
};

enum class ExpNodeType {
    BINOP,
    PARTIAL,
    BOOLEAN,
    DEF_CONST,
    CALL,
    METHOD,
    CAST,
    DICT,
    EMPTYDICT,
    CLS,
    EMPTYLST,
    ID,
    LST,
    MEMBER,
    NONE,
    NUMBER,
    STRNG,
    SUB,
    TERNARY,
    TUPLE,
    BOOLOP,
    MATCH_EXP,
    ALIAS,
    UNARY,
};


enum class TopNodeType {
    IMPORT, CLS, FUNC, ENUM, TYPECLASS,
};

#endif //UNTITLED1_NODE_TYPES_H
