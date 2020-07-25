//
// Created by chris on 20/7/20.
//

#ifndef UNTITLED1_OBJECT_H
#define UNTITLED1_OBJECT_H

#include "ObjectStack.h"
#include "Inst.h"
enum class ValueType {
    INTEGER, CODE
};

class Inst;
class Integer;
class Code;
class Object {
public:
    Integer* integer;
    Code* code;
    ValueType type;

    Object(Integer* integer);

    Object(Code* code);
};

class Integer {
public:
    Integer(int value);

    Integer();

    int value;
};

class CodeBuiltin {
public:
    virtual void run(ObjectStack* stack) = 0;
};

class BuiltinSum : public CodeBuiltin {
public:
    void run(ObjectStack* stack);
};

class CodeUser {
public:
    std::vector<Inst*> code;

    CodeUser(std::vector<Inst*> code);

    Inst* get(int i);

    size_t size();
};

enum class CodeType {
    BUILTIN, USER
};

class Code {
    union {
        CodeUser* user;
    };

public:
    Code(CodeBuiltin* builtin);

    CodeType type;
    CodeBuiltin* builtin;
};

bool equal(Object* a, Object* b);


#endif //UNTITLED1_OBJECT_H
