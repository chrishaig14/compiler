//
// Created by chris on 20/7/20.
//

#ifndef UNTITLED1_FRAME_H
#define UNTITLED1_FRAME_H


#include <vector>
#include <stack>
#include <stdexcept>
#include "Inst.h"
#include "Value.h"
#include "ValueStack.h"
#include "Environment.h"


class Frame {
public:

    std::vector<Inst*> code;
    size_t inst_ptr;
    ValueStack* stack;
    Environment* env;

    void run() {
        while (inst_ptr < code.size()) {
            Inst* inst = this->code[this->inst_ptr];
            this->run_inst(inst);
        }
    }

    void run_inst(PopInst* pop) {
        this->stack->pop();
    }

    void run_inst(CallInst* call) {}

    void run_inst(Inst* inst) {
        switch (inst->type) {
            case InstType::PUSH:
                this->run_inst(inst->push);
                break;
            case InstType::POP:
                this->run_inst(inst->pop);
                break;
            case InstType::CALL:
                this->run_inst(inst->call);
                break;
            case InstType::LOAD:
                this->run_inst(inst->load);
                break;
            case InstType::STORE:
                this->run_inst(inst->store);
                break;
            case InstType::DECLARE:
                this->run_inst(inst->declare);
                break;
        }
    }

    void run_inst(DeclareInst* declare) {
        this->env->declare(declare->name);
    }

    Frame(std::vector<Inst*> code, ValueStack* stack) {
        this->stack = stack;
        this->env = new Environment(nullptr);
    }

    void run_inst(PushInst* push) {
        this->stack->push(push->value);
    }

    void run_inst(StoreInst* store) {
        Value* value = this->stack->pop();
        this->env->set(store->name, value);
    }

    void run_inst(LoadInst* load) {
        Value* value = this->env->get(load->name);
        this->stack->push(value);
    }
};


#endif //UNTITLED1_FRAME_H
