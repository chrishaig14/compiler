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
#include "UserObject.h"

class CodeRunner : public InstructionVisitor {
public:

    Code& code;
    size_t inst_ptr;
    ObjectStack& stack;
    Environment* env;

    CodeRunner(Code& code, ObjectStack& stack, std::map<std::string, Object*> closure) : code(code), stack(stack) {
        this->env = new Environment(nullptr);
        for (auto it: closure) {
            this->env->declare(it.first);
            this->env->set(it.first, it.second);
        }
        this->inst_ptr = 0;
    }

    void run() {
        while (this->inst_ptr < this->code.size()) {
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
            CodeRunner code_runner(code->user->code, this->stack, code->user->closure);
            code_runner.run();
//            throw std::runtime_error("Trying to run user code!");
        }
        this->inst_ptr++;
//        throw std::runtime_error("Unimplemented call instruction!");
    }

    void visit(DeclareInst& declare) override {
        this->env->declare(declare.identifier);
        this->inst_ptr++;
    }

    void visit(GetInst& inst) override {
        Object* value = this->env->get(inst.identifier);
        this->stack.push(value);
        this->inst_ptr++;
    }

    void visit(GetMemberInst& inst) override {
        Object* object = this->stack.pop();
        UserObject* user_object = dynamic_cast<UserObject*>(object);
        this->stack.push(user_object->fields[inst.member]);
        this->inst_ptr++;
    }

    void visit(GetSubscriptInst& inst) override {
        this->inst_ptr++;
    }

    void visit(PushFunctionInst& inst) override {
        std::map<std::string, Object*> closure;
        for (auto it: inst.free_variables) {
            closure[it.first] = this->env->get(it.first);
        }
        this->stack.push(new CodeObject(new CodeUser(inst.body, closure)));
        this->inst_ptr++;
    }

    void visit(PushIntegerInst& inst) override {
        IntegerObject* reference_to_new_integer_object = new IntegerObject(inst.num);
        this->stack.push(reference_to_new_integer_object);
        this->inst_ptr++;
    }

    void visit(PushStringInst& inst) override {
        StringObject* reference_to_new_integer_object = new StringObject(inst.str);
        this->stack.push(reference_to_new_integer_object);
        this->inst_ptr++;
    }

    void visit(ReturnInst& inst) override {
        this->inst_ptr++;
    }

    void visit(SetInst& inst) override {
        Object* value = this->stack.pop();
        this->env->set(inst.identifier, value);
        this->inst_ptr++;
    }

    void visit(SetMemberInst& inst) override {
        Object* object = this->stack.pop();
        UserObject* user_object = dynamic_cast<UserObject*>(object);
        user_object->fields[inst.member] = this->stack.pop();
        this->inst_ptr++;
    }

    void visit(SetSubscriptInst& inst) override {

    }

    void visit(MakeObjectInst& inst) override {
        this->stack.push(new UserObject(inst.type, inst.fields));
        this->inst_ptr++;
    }

};


#endif //UNTITLED1_CODERUNNER_H
