//
// Created by chris on 20/7/20.
//

#ifndef OBJECT_H
#define OBJECT_H

enum class ValueType {
    INTEGER, CODE
};

class ObjectStack;

class Instruction;

class IntegerObject;

class StringObject;

class CodeObject;

class Environment;

#define SET_FLAG(bitfield, flag) bitfield |= flag
#define UNSET_FLAG(bitfield, flag) bitfield &= ~(flag)
#define FLAG_IS_SET(bitfield, flag) (bitfield & (flag))
#define VISITED (1<<7)
#define INSERTED (1<<6)
#define IS_USER (1<<5)
#define IS_INT (1<<4)
#define IS_LIST (1<<3)
#define REACHABLE (1<<2)

class Object {
public:
    Object() {
    }

    virtual bool equal(const Object* other) const = 0;

    virtual Object* sum(const Object* other) const { return nullptr; }

    virtual ~Object() {}
};

enum class CodeType {
    BUILTIN, USER
};


bool equal(Object* a, Object* b);


#endif //OBJECT_H
