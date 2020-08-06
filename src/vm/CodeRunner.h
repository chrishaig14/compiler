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
#include "ListObject.h"
#include "CodeObject.h"
#include "UserObject.h"
#include "BooleanObject.h"

class CodeRunner : public InstructionVisitor {
public:

    Code& code;
    size_t inst_ptr;
    ObjectStack& stack;
    Environment* env;

    CodeRunner(Code& code, ObjectStack& stack, std::map<std::string, Object*> closure) : code(code), stack(stack) {
        std::cout << "New code runner" << std::endl;
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
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

        Object* right = this->stack.pop();
        Object* left = this->stack.pop();
        IntegerObject* right_int = dynamic_cast<IntegerObject*>(right);
        IntegerObject* left_int = dynamic_cast<IntegerObject*>(left);
        int result = 0;
        if (right_int != nullptr && left_int != nullptr) {
            switch (inst.op) {
                case OpType::ADD:
                    result = left_int->value + right_int->value;
                    this->stack.push(new IntegerObject(result));

                    break;
                case OpType::SUB:
                    result = left_int->value - right_int->value;
                    this->stack.push(new IntegerObject(result));

                    break;
                case OpType::MUL:
                    result = left_int->value * right_int->value;
                    this->stack.push(new IntegerObject(result));

                    break;
                case OpType::DIV:
                    result = left_int->value / right_int->value;
                    this->stack.push(new IntegerObject(result));

                    break;
                case OpType::EQ:
                    result = left_int->value == right_int->value;
                    this->stack.push(new BooleanObject(result));
                    break;
                case OpType::AND:
//                    result = left_int->value + right_int->value;
                    break;
                case OpType::OR:
//                    result = left_int->value + right_int->value;
                    break;
            }
        } else {
            throw std::runtime_error("Try to do a binop with two non-Integers");
        }
        this->inst_ptr++;
    }

    void visit(CallInst& call) override {
        std::cout << "Run [" << call.to_string() << "]" << std::endl;

        Object* value = this->stack.pop();
        CodeObject* code = dynamic_cast<CodeObject*>(value);
        if (code == nullptr) {
            throw std::runtime_error("Trying to call something that's not code!");
        }
        if (code->type == CodeType::BUILTIN) {
            code->builtin->run(this->stack);
        } else {
            CodeRunner code_runner(code->user->code, this->stack, {});
            for (auto v: code->user->closure) {
                code_runner.env->declare(v);
                code_runner.env->set(v, this->env->get(v));
            }
            code_runner.run();
//            throw std::runtime_error("Trying to run user code!");
        }
        this->inst_ptr++;
//        throw std::runtime_error("Unimplemented call instruction!");
    }

    void visit(DeclareInst& declare) override {
        std::cout << "Run [" << declare.to_string() << "]" << std::endl;
        this->env->declare(declare.identifier);
        this->inst_ptr++;
    }

    void visit(GetInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;
        Object* value = this->env->get(inst.identifier);
        this->stack.push(value);
        this->inst_ptr++;
    }

    void visit(GetMemberInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;
        Object* object = this->stack.pop();
        UserObject* user_object = dynamic_cast<UserObject*>(object);
        this->stack.push(user_object->fields[inst.member]);
        this->inst_ptr++;
    }

    void visit(GetSubscriptInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

        this->inst_ptr++;
    }

    void visit(PushFunctionInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

        this->stack.push(new CodeObject(new CodeUser(inst.body, inst.free_variables)));
        this->inst_ptr++;
    }

    void visit(PushIntegerInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

        IntegerObject* reference_to_new_integer_object = new IntegerObject(inst.num);
        this->stack.push(reference_to_new_integer_object);
        this->inst_ptr++;
    }

    void visit(PushStringInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

        StringObject* reference_to_new_integer_object = new StringObject(inst.str);
        this->stack.push(reference_to_new_integer_object);
        this->inst_ptr++;
    }

    void visit(ReturnInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

        this->inst_ptr = this->code.size() + 1;
    }

    void visit(SetInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

        Object* value = this->stack.pop();
        this->env->set(inst.identifier, value);
        this->inst_ptr++;
    }

    void visit(SetMemberInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

        Object* object = this->stack.pop();
        UserObject* user_object = dynamic_cast<UserObject*>(object);
        user_object->fields[inst.member] = this->stack.pop();
        this->inst_ptr++;
    }

    void visit(SetSubscriptInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

    }

    void visit(MakeObjectInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

        this->stack.push(new UserObject(inst.type, inst.fields));
        this->inst_ptr++;
    }

    void visit(MakeListInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

        std::vector<Object*> list(inst.length, nullptr);
        for (int i = inst.length - 1; i >= 0; i--) {
            list[i] = this->stack.pop();
        }
        this->stack.push(new ListObject(list));
        this->inst_ptr++;
    }

    void visit(JumpIfFalseInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

        Object* tos = this->stack.pop();
        BooleanObject* boolean_ptr = dynamic_cast<BooleanObject*>(tos);
        if (boolean_ptr == nullptr) {
            throw std::runtime_error("Expected boolean!");
        }
        if (!boolean_ptr->boolean) {
            this->inst_ptr += inst.offset;
        } else {
            this->inst_ptr++;
        }
    }

    void visit(PushBooleanInst& inst) override {
        std::cout << "Run [" << inst.to_string() << "]" << std::endl;

        this->stack.push(new BooleanObject(inst.boolean));
        this->inst_ptr++;
    }

};


#endif //UNTITLED1_CODERUNNER_H
