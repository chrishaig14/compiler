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

class Object {
public:
    virtual bool equal(const Object* other) const = 0;

    virtual Object* sum(const Object* other) const { return nullptr; }

    virtual ~Object() {}
};

typedef void(* BuiltinFunction)(std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs,
                                ObjectStack& stack, Environment* global_env);


struct CodeBuiltin {
    FunctionTypeNode* ftype;

    BuiltinFunction function;
};


enum class CodeType {
    BUILTIN, USER
};


bool equal(Object* a, Object* b);


#endif //OBJECT_H
