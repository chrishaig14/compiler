//
// Created by chris on 20/7/20.
//

#ifndef UNTITLED1_OBJECT_H
#define UNTITLED1_OBJECT_H

#include "../utils.h"

enum class ValueType {
    INTEGER, CODE
};

class ObjectStack;

class Instruction;

class IntegerObject;

class StringObject;

class CodeObject;

class Object {
public:
    virtual bool equal(const Object* other) const = 0;

    virtual Object* sum(const Object* other) const { return nullptr; }
};


class CodeBuiltin {
public:
    FunctionTypeNode* ftype;
    virtual void run(ObjectStack& stack) = 0;
};


enum class CodeType {
    BUILTIN, USER
};


bool equal(Object* a, Object* b);


#endif //UNTITLED1_OBJECT_H
