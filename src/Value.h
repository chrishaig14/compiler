//
// Created by chris on 20/7/20.
//

#ifndef UNTITLED1_VALUE_H
#define UNTITLED1_VALUE_H

#include "ValueStack.h"
#include "Inst.h"
enum class ValueType {
    INTEGER, CODE
};

class Inst;
class Integer;
class Code;
class Value {
public:
    Integer* integer;
    Code* code;
    ValueType type;

    Value(Integer* integer);

    Value(Code* code);
};

class Integer {
public:
    Integer(int value);

    Integer();

    int value;
};

class CodeBuiltin {
public:
    virtual void run(ValueStack* stack) = 0;
};

class BuiltinSum : public CodeBuiltin {
public:
    void run(ValueStack* stack);
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

bool equal(Value* a, Value* b);


#endif //UNTITLED1_VALUE_H
