//
// Created by chris on 4/4/21.
//

#ifndef XLANG_SNODE_H
#define XLANG_SNODE_H

enum class SNodeType {
    FUNCTION, ID, DECLARATION, BLOCK, ASSIGNMENT, RETURN, INTEGER, CALL, STRING, BOOLEAN, FLOAT, CLASS, NEW,
    OBJECT_MEMBER, WHILE, LIST, IF, BREAK, CONTINUE, MATCH, ENUM, ENUM_MEMBER, NONE, TERNARY, DICT
};

class SNode {
public:
    SNodeType type;
    SNode(SNodeType type);
};


#endif //XLANG_SNODE_H
