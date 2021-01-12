//
// Created by chris on 20/7/20.
//

#ifndef OBJECT_H
#define OBJECT_H

//#include "../utils.h"

#include "../nodes/TypeNode.h"
#include "../utils.h"

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
    uint8_t flags;

//    bool visited;
    Object() {
        this->flags = 0;
    }

    virtual bool equal(const Object* other) const = 0;

    virtual Object* sum(const Object* other) const { return nullptr; }

    virtual ~Object() {}
};

typedef void(* BuiltinFunction)(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs,
                                ObjectStack& stack, Environment* global_env);


struct CodeBuiltin {
    FunctionType* ftype;

    BuiltinFunction function;
};


enum class CodeType {
    BUILTIN, USER
};


bool equal(Object* a, Object* b);


#endif //OBJECT_H
