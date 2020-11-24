//
// Created by chris on 20/7/20.
//

#include "CodeRunner.h"
#include "NoneObject.h"
#include <iostream>

CodeRunner::CodeRunner(const Code& code,
                       std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs,
                       ObjectStack& stack, Environment* global_env) : code(code), stack(stack),
                                                                      structs(structs) {
    //std::cerr << "New code runner" << std::endl;
    this->env = new Environment("", global_env);
    this->global_env = global_env;
    this->inst_ptr = 0;
}

void CodeRunner::run() {
    while (this->inst_ptr < this->code.size()) {
        Instruction* inst = this->code[this->inst_ptr];
        inst->accept(*this);
    }
}

void CodeRunner::visit(BinopInst& inst) {
//    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

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
            case OpType::MOD:
                result = left_int->value % right_int->value;
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
            case OpType::LEQ:
                result = left_int->value <= right_int->value;
                this->stack.push(new BooleanObject(result));
                break;
            case OpType::GEQ:
                result = left_int->value >= right_int->value;
                this->stack.push(new BooleanObject(result));
                break;
            case OpType::LT:
                result = left_int->value < right_int->value;
                this->stack.push(new BooleanObject(result));
                break;
            case OpType::GT:
                result = left_int->value > right_int->value;
                this->stack.push(new BooleanObject(result));
                break;
            case OpType::NEQ:
                result = left_int->value != right_int->value;
                this->stack.push(new BooleanObject(result));
                break;
        }
    } else {
        StringObject* right_str = dynamic_cast<StringObject*>(right);
        StringObject* left_str = dynamic_cast<StringObject*>(left);
        std::string result;
        if (right_str != nullptr && left_str != nullptr) {
            if (inst.op == OpType::ADD) {
                result = left_str->str + right_str->str;
                this->stack.push(new StringObject(result));
            } else if (inst.op == OpType::EQ) {
                bool res = left_str->str == right_str->str;
                this->stack.push(new BooleanObject(res));
            } else if (inst.op == OpType::NEQ) {
                bool res = left_str->str != right_str->str;
                this->stack.push(new BooleanObject(res));
            }
        } else {
            ListObject* right_list = dynamic_cast<ListObject*>(right);
            ListObject* left_list = dynamic_cast<ListObject*>(left);
            ListObject* result;
            if (right_list != nullptr && left_list != nullptr) {
                if (inst.op == OpType::ADD) {
                    std::vector<Object*> elements;
                    elements.insert(elements.end(), left_list->list.begin(), left_list->list.end());
                    elements.insert(elements.end(), right_list->list.begin(), right_list->list.end());
                    result = new ListObject(elements);
                    this->stack.push(result);
                }
            } else {
                NoneObject* right_none = dynamic_cast<NoneObject*>(right);
                if (right_none != nullptr) {
                    NoneObject* left_none = dynamic_cast<NoneObject*>(left);
                    if (inst.op == OpType::EQ) {
                        this->stack.push(new BooleanObject(left_none != nullptr));
                    } else if (inst.op == OpType::NEQ) {
                        this->stack.push(new BooleanObject(left_none == nullptr));
                    }

                } else {
                    throw std::runtime_error("Try to do a binop with two non-Integers or non-Strings");
                }
            }
        }
    }
    this->inst_ptr++;
}

void CodeRunner::visit(CallInst& call) {
//    //std::cerr << "Run [" << call.to_string() << "]" << std::endl;

    Object* value = this->stack.pop();
    CodeObject* code = dynamic_cast<CodeObject*>(value);
    if (code == nullptr) {
        throw std::runtime_error("Trying to call something that's not code!");
    }
    if (code->type == CodeType::BUILTIN) {
        code->stuff.builtin->function(this->structs, this->stack, this->global_env);
    } else {
        CodeRunner code_runner(code->stuff.user->code, this->structs, this->stack, this->global_env);
        code_runner.run();
//            throw std::runtime_error("Trying to run user code!");
    }
    this->inst_ptr++;
//        throw std::runtime_error("Unimplemented call instruction!");
}

void CodeRunner::visit(DeclareInst& declare) {
    //std::cerr << "Run [" << declare.to_string() << "]" << std::endl;
    this->env->declare(declare.identifier);
    this->inst_ptr++;
}

void CodeRunner::visit(GetInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;
    Object* value = this->env->get(inst.identifier);
    this->stack.push(value);
    this->inst_ptr++;
}

void CodeRunner::visit(GetMemberInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;
    Object* object = this->stack.pop();
    UserObject* user_object = dynamic_cast<UserObject*>(object);
    this->stack.push(user_object->fields[inst.s_member]);
    this->inst_ptr++;
}

void CodeRunner::visit(GetTupleMemberInst& inst) {
    std::cerr << "Run [" << inst.to_string() << "]" << std::endl;
    TupleObject* object = this->stack.pop_tuple();
    this->stack.push(object->values[inst.member - 1]);
    this->inst_ptr++;
}

void CodeRunner::visit(GetSubscriptInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;
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
}

void CodeRunner::visit(PushIntegerInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    IntegerObject* reference_to_new_integer_object = new IntegerObject(inst.num);
    this->stack.push(reference_to_new_integer_object);
    this->inst_ptr++;
}

void CodeRunner::visit(PushStringInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    StringObject* reference_to_new_integer_object = new StringObject(inst.str);
    this->stack.push(reference_to_new_integer_object);
    this->inst_ptr++;
}

void CodeRunner::visit(ReturnInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    this->inst_ptr = this->code.size() + 1;
}

void CodeRunner::visit(SetInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    Object* value = this->stack.pop();
    this->env->set(inst.identifier, value);
    this->inst_ptr++;
}

void CodeRunner::visit(SetMemberInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    Object* object = this->stack.pop();
    UserObject* user_object = dynamic_cast<UserObject*>(object);
    user_object->fields[inst.member] = this->stack.pop();
    this->inst_ptr++;
}

void CodeRunner::visit(SetSubscriptInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;
    Object* element = this->stack.pop();
    Object* index = this->stack.pop();
    Object* value = this->stack.pop();
    ListObject* list = dynamic_cast<ListObject*>(element);
    IntegerObject* pos = dynamic_cast<IntegerObject*>(index);
    list->list[pos->value] = value;
    this->inst_ptr++;
}

void CodeRunner::visit(MakeObjectInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;
    UserObject* obj = new UserObject(inst.type, inst.fields);
    for (int i = inst.fields.size() - 1; i >= 0; i--) {
        obj->fields[inst.fields[i]] = this->stack.pop();
    }
    this->stack.push(obj);
    this->inst_ptr++;
}

void CodeRunner::visit(MakeListInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    std::vector<Object*> list(inst.length, nullptr);
    for (int i = inst.length - 1; i >= 0; i--) {
        list[i] = this->stack.pop();
    }
    this->stack.push(new ListObject(list));
    this->inst_ptr++;
}

void CodeRunner::visit(JumpIfFalseInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

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
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

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
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;
    this->inst_ptr += inst.offset;
}

void CodeRunner::visit(LeaveScope& inst) {
    Environment* old_env = this->env;
    this->env = this->env->leave(inst.name);
    delete old_env;
    this->inst_ptr++;
}

void CodeRunner::visit(EnterScope& inst) {
    this->env = this->env->enter(inst.name);
    this->inst_ptr++;
}

void CodeRunner::visit(NopInst& inst) {
    this->inst_ptr++;
}

void CodeRunner::visit(EndFunction& inst) {
    // nothing
}

void CodeRunner::visit(StartFunction& inst) {
    // nothing
}

void CodeRunner::visit(JumpIfNone& inst) {
    Object* tos = this->stack.top();
    if (tos->equal(new NoneObject())) {
        this->stack.pop();
        this->inst_ptr += inst.offset;
    } else {
        this->inst_ptr++;
    }
}

void CodeRunner::visit(PushNone& inst) {
    this->stack.push(new NoneObject());
    this->inst_ptr++;
}

void CodeRunner::visit(PopInst& inst) {
    this->stack.pop();
    this->inst_ptr++;
}

void CodeRunner::visit(MakeTupleInst& inst) {
    std::vector<Object*> tuple(inst.length, nullptr);
    for (int i = inst.length - 1; i >= 0; i--) {
        tuple[i] = this->stack.pop();
    }
    this->stack.push(new TupleObject(tuple));
    this->inst_ptr++;
}
