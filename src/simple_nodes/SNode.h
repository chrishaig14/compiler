//
// Created by chris on 4/4/21.
//

#ifndef XLANG_SNODE_H
#define XLANG_SNODE_H

#include <memory>
#include "sem.h"

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
    TRY_CATCH, OBJECT_METHOD_CALL, OBJECT_METHOD
};

class sem::SNode {
public:
    SNodeType type;
    explicit SNode(SNodeType type);

    virtual bool equals(const SNode& o) const = 0;

    bool operator!=(const SNode& other) const {
        return !(*this == other);
    }

    bool operator==(const SNode& other) const {
        if (this->type != other.type) {
            return false;
        }
        return this->equals(other);
    }

    virtual ~SNode() = default;
};

typedef std::unique_ptr<sem::SNode> USNode;

#endif //XLANG_SNODE_H
