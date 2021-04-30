//
// Created by chris on 17/11/20.
//

#ifndef UNTITLED1_NODE_TYPES_H
#define UNTITLED1_NODE_TYPES_H

enum class NodeType {
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
    FLOT,
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
    BOOLOP, MATCH_EXP, ALIAS, ENUM
};

#endif //UNTITLED1_NODE_TYPES_H
