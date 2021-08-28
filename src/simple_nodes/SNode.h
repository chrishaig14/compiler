//
// Created by chris on 4/4/21.
//

#ifndef XLANG_SNODE_H
#define XLANG_SNODE_H

#include <memory>

enum class SNodeType {
    FUNCTION,
    ID,
    DECLARATION,
    BLOCK,
    ASSIGNMENT,
    RETURN,
    INTEGER,
    CALL,
    STRING,
    BOOLEAN,
    FLOAT,
    CLASS,
    NEW,
    OBJECT_MEMBER,
    WHILE,
    LIST,
    IF,
    BREAK,
    CONTINUE,
    MATCH,
    ENUM,
    ENUM_MEMBER,
    NONE,
    TERNARY,
    DICT,
    THROW,
    TRY_CATCH
};

class SNode {
public:
    SNodeType type;
    explicit SNode(SNodeType type);
};

typedef std::unique_ptr<SNode> USNode;

#endif //XLANG_SNODE_H
