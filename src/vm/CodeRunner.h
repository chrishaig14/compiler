//
// Created by chris on 20/7/20.
//

#ifndef UNTITLED1_CODERUNNER_H
#define UNTITLED1_CODERUNNER_H


#include <vector>
#include <stack>
#include <stdexcept>
#include "instructions/instructions.h"
#include "Object.h"
#include "ObjectStack.h"
#include "Environment.h"
#include "IntegerObject.h"
#include "StringObject.h"
#include "CodeObject.h"

class CodeRunner : public InstructionVisitor {
public:

    Code& code;
    size_t inst_ptr;
    ObjectStack& stack;
    Environment* env;

    CodeRunner(Code& code, ObjectStack& stack) : code(code), stack(stack) {
        this->env = new Environment(nullptr);
    }

    void run() {
        while (inst_ptr < code.size()) {
            Instruction* inst = this->code[this->inst_ptr];
            inst->accept(*this);
        }
    }

    void visit(BinopInst& inst) override {

    }

    void visit(CallInst& call) override {
        Object* value = this->stack.pop();
        CodeObject* code = dynamic_cast<CodeObject*>(value);
        if (code == nullptr) {
            throw std::runtime_error("Trying to call something that's not code!");
        }
        if (code->type == CodeType::BUILTIN) {
            code->builtin->run(this->stack);
        } else {
            throw std::runtime_error("Trying to run user code!");
        }
//        throw std::runtime_error("Unimplemented call instruction!");
    }

    void visit(DeclareInst& declare) override {
        this->env->declare(declare.identifier);
    }

    void visit(GetInst& inst) override {
        Object* value = this->env->get(inst.identifier);
        this->stack.push(value);
    }

    void visit(GetMemberInst& inst) override {

    }

    void visit(GetSubscriptInst& inst) override {

    }

    void visit(PushFunctionInst& inst) override {

    }

    void visit(PushIntegerInst& inst) override {
        IntegerObject* reference_to_new_integer_object = new IntegerObject(inst.num);
        this->stack.push(reference_to_new_integer_object);
    }

    void visit(PushStringInst& inst) override {
        StringObject* reference_to_new_integer_object = new StringObject(inst.str);
        this->stack.push(reference_to_new_integer_object);
    }

    void visit(ReturnInst& inst) override {

    }

    void visit(SetInst& inst) override {
        Object* value = this->stack.pop();
        this->env->set(inst.identifier, value);
    }

    void visit(SetMemberInst& inst) override {

    }

    void visit(SetSubscriptInst& inst) override {

    }

};


#endif //UNTITLED1_CODERUNNER_H
