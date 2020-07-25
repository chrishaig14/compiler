//
// Created by chris on 20/7/20.
//

#ifndef UNTITLED1_INST_H
#define UNTITLED1_INST_H

#include <string>
class Value;
enum class InstType {
    PUSH,
    POP,
    CALL,
    LOAD,
    STORE,
    DECLARE
};

class PushInst {
public:
    Value* value;

    PushInst(Value* value) {
        this->value = value;
    }
};

class PopInst {
};

class CallInst {
};

class LoadInst {
public:
    std::string name;

    LoadInst(std::string name) {
        this->name = name;
    }
};


class DeclareInst {
public:
    std::string name;
    DeclareInst(std::string
    name){
        this->name = name;
    }
};

class StoreInst {
public:
    std::string name;

    StoreInst(std::string name) {
        this->name = name;
    }
};

class Inst {

public:
    InstType type;
    LoadInst* load;
    StoreInst* store;
    PushInst* push;
    PopInst* pop;
    CallInst* call;
    DeclareInst* declare;
};

#endif //UNTITLED1_INST_H
