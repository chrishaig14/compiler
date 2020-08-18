//
// Created by chris on 20/7/20.
//

#include "CodeRunner.h"
#include <iostream>

CodeRunner::CodeRunner(const Code& code, std::map<std::string, std::map<std::string, Code>>& structs,
                       ObjectStack& stack, std::map<std::string, Object*> closure) : code(code), stack(stack),
                                                                                     structs(structs) {
    std::cout << "New code runner" << std::endl;
    this->env = new Environment(nullptr);
    for (auto it: closure) {
        this->env->declare(it.first);
        this->env->set(it.first, it.second);
    }
    this->inst_ptr = 0;
}

void CodeRunner::run() {
    while (this->inst_ptr < this->code.size()) {
        Instruction* inst = this->code[this->inst_ptr];
        inst->accept(*this);
    }
}

void CodeRunner::visit(BinopInst& inst) {
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

void CodeRunner::visit(CallInst& call) {
    std::cout << "Run [" << call.to_string() << "]" << std::endl;

    Object* value = this->stack.pop();
    CodeObject* code = dynamic_cast<CodeObject*>(value);
    if (code == nullptr) {
        throw std::runtime_error("Trying to call something that's not code!");
    }
    if (code->type == CodeType::BUILTIN) {
        code->builtin->run(this->stack);
    } else {
        CodeRunner code_runner(code->user->code, this->structs, this->stack, {});
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

void CodeRunner::visit(DeclareInst& declare) {
    std::cout << "Run [" << declare.to_string() << "]" << std::endl;
    this->env->declare(declare.identifier);
    this->inst_ptr++;
}

void CodeRunner::visit(GetInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;
    Object* value = this->env->get(inst.identifier);
    this->stack.push(value);
    this->inst_ptr++;
}

void CodeRunner::visit(GetMemberInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;
    Object* object = this->stack.pop();
    UserObject* user_object = dynamic_cast<UserObject*>(object);
    this->stack.push(user_object->fields[inst.member]);
    this->inst_ptr++;
}

void CodeRunner::visit(GetSubscriptInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;
    Object* obj = this->stack.pop();
    ListObject* list = dynamic_cast<ListObject*>(obj);
    if (list == nullptr) {
        throw std::runtime_error("Subscript of non-list");
    }
    obj = this->stack.pop();
    IntegerObject* index = dynamic_cast<IntegerObject*>(obj);
    if (index == nullptr) {
        throw std::runtime_error("Non-integer subscript of list");
    }
    this->stack.push(list->list[index->value]);
    this->inst_ptr++;
}

void CodeRunner::visit(PushFunctionInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;
    Code function_body = inst.body;
    Code get_parameters;
    for (int i = inst.parameter_names.size() - 1; i >= 0; i--) {
        Code param_code = {I_DECL(inst.parameter_names[i]), I_SET(inst.parameter_names[i])};
        get_parameters.insert(get_parameters.end(), param_code.begin(), param_code.end());
    }
    function_body.insert(function_body.begin(), get_parameters.begin(), get_parameters.end());
    this->stack.push(new CodeObject(new CodeUser(function_body, inst.free_variables)));
    this->inst_ptr++;
}

void CodeRunner::visit(PushIntegerInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;

    IntegerObject* reference_to_new_integer_object = new IntegerObject(inst.num);
    this->stack.push(reference_to_new_integer_object);
    this->inst_ptr++;
}

void CodeRunner::visit(PushStringInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;

    StringObject* reference_to_new_integer_object = new StringObject(inst.str);
    this->stack.push(reference_to_new_integer_object);
    this->inst_ptr++;
}

void CodeRunner::visit(ReturnInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;

    this->inst_ptr = this->code.size() + 1;
}

void CodeRunner::visit(SetInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;

    Object* value = this->stack.pop();
    this->env->set(inst.identifier, value);
    this->inst_ptr++;
}

void CodeRunner::visit(SetMemberInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;

    Object* object = this->stack.pop();
    UserObject* user_object = dynamic_cast<UserObject*>(object);
    user_object->fields[inst.member] = this->stack.pop();
    this->inst_ptr++;
}

void CodeRunner::visit(SetSubscriptInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;

}

void CodeRunner::visit(MakeObjectInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;
    UserObject* obj = new UserObject(inst.type, inst.fields);
    for (int i = inst.fields.size() - 1; i >= 0; i--) {
        obj->fields[inst.fields[i]] = this->stack.pop();
    }
    this->stack.push(obj);
    this->inst_ptr++;
}

void CodeRunner::visit(MakeListInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;

    std::vector<Object*> list(inst.length, nullptr);
    for (int i = inst.length - 1; i >= 0; i--) {
        list[i] = this->stack.pop();
    }
    this->stack.push(new ListObject(list));
    this->inst_ptr++;
}

void CodeRunner::visit(JumpIfFalseInst& inst) {
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

void CodeRunner::visit(PushBooleanInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;

    this->stack.push(new BooleanObject(inst.boolean));
    this->inst_ptr++;
}

void CodeRunner::visit(MakeClassInst& inst) {
    this->classes[inst.identifier] = inst.fields;
    this->inst_ptr++;
}

void CodeRunner::visit(DefineStructInst& inst) {
    this->structs[inst.identifier] = inst.fields;
    this->inst_ptr++;
}

void CodeRunner::visit(MakeDefaultInst& inst) {
//        if (inst.identifier == "String") {
//            this->stack.push(new StringObject(""));
//        } else if (inst.identifier == "Integer") {
//            this->stack.push(new IntegerObject());
//        } else {
//            auto proto = this->structs[inst.identifier];
//            std::vector<std::string> names;
//            for(auto f: proto){
//                names.push_back(f.first);
//                CodeRunner field_initializer(f.second,this->structs, this->stack, {});
//            }
//            UserObject* obj = new UserObject(inst.identifier, names);
//            obj->fields;
//        }
//        this->inst_ptr++;
}

void CodeRunner::visit(JumpInst& inst) {
    std::cout << "Run [" << inst.to_string() << "]" << std::endl;
    this->inst_ptr += inst.offset;
}
