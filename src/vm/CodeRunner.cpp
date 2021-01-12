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
    this->env->global = global_env;
    this->global_env = global_env;
    this->inst_ptr = 0;
}

void CodeRunner::run() {
    while (this->inst_ptr < this->code.size()) {
        Instruction* inst = this->code[this->inst_ptr];
        inst->accept(*this);
    }
}


bool bool_int_int(BoolOp op, int a, int b) {
    switch (op) {
        case BoolOp::EQ:
            return a == b;
        case BoolOp::LEQ:
            return a <= b;
        case BoolOp::GEQ:
            return a >= b;
        case BoolOp::LT:
            return a < b;
        case BoolOp::GT:
            return a > b;
        case BoolOp::NEQ:
            return a != b;
        default:
            throw std::runtime_error("Unsupported op between int and int");
    }
}

bool bool_float_float(BoolOp op, float a, float b) {
    switch (op) {
        case BoolOp::EQ:
            return a == b;
        case BoolOp::LEQ:
            return a <= b;
        case BoolOp::GEQ:
            return a >= b;
        case BoolOp::LT:
            return a < b;
        case BoolOp::GT:
            return a > b;
        case BoolOp::NEQ:
            return a != b;
        default:
            throw std::runtime_error("Unsupported op between int and int");
    }
}

bool bool_str_str(BoolOp op, std::string a, std::string b) {
    switch (op) {
        case BoolOp::EQ:
            return a == b;
        case BoolOp::LEQ:
            return a <= b;
        case BoolOp::GEQ:
            return a >= b;
        case BoolOp::LT:
            return a < b;
        case BoolOp::GT:
            return a > b;
        case BoolOp::NEQ:
            return a != b;
        default:
            throw std::runtime_error("Unsupported op between int and int");
    }
}

bool bool_op(BoolOp op, Object* left, Object* right) {
    IntegerObject* right_int = dynamic_cast<IntegerObject*>(right);
    IntegerObject* left_int = dynamic_cast<IntegerObject*>(left);
    if (right_int != nullptr && left_int != nullptr) {
        return bool_int_int(op, left_int->value, right_int->value);
    }
    FloatObject* right_float = dynamic_cast<FloatObject*>(right);
    FloatObject* left_float = dynamic_cast<FloatObject*>(left);
    if (right_float != nullptr && left_float != nullptr) {
        return bool_float_float(op, left_float->value, right_float->value);
    }
    StringObject* right_str = dynamic_cast<StringObject*>(right);
    StringObject* left_str = dynamic_cast<StringObject*>(left);
    if (right_str != nullptr && left_str != nullptr) {
        return bool_str_str(op, left_str->str, right_str->str);
    }
}

void CodeRunner::visit(BoolOpInst& inst) {
//    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    Object* right = this->stack.pop();
    Object* left = this->stack.pop();
    BooleanObject* obj = new BooleanObject(bool_op(inst.op, left, right));
    ObjectStore::register_object(obj);
    this->stack.push(obj);
    this->inst_ptr++;
    return;
}

int op_int_int(OpType op, int a, int b) {
    switch (op) {
        case OpType::ADD:
            return a + b;
        case OpType::SUB:
            return a - b;
        case OpType::MUL:
            return a * b;
        case OpType::DIV:
            return a / b;
        case OpType::MOD:
            return a % b;
    }
}

std::string op_str_str(OpType op, std::string a, std::string b) {
    switch (op) {
        case OpType::ADD:
            return a + b;
        default:
            throw std::runtime_error("Unsupported op between two strings!");
    }
}

float op_float_float(OpType op, float a, float b) {
    switch (op) {
        case OpType::ADD:
            return a + b;
        case OpType::SUB:
            return a - b;
        case OpType::MUL:
            return a * b;
        case OpType::DIV:
            return a / b;
        default:
            throw std::runtime_error("Unsupported op between two floats!");
    }
}

float op_float_int(OpType op, float a, int b) {
    switch (op) {
        case OpType::ADD:
            return a + b;
        case OpType::SUB:
            return a - b;
        case OpType::MUL:
            return a * b;
        case OpType::DIV:
            return a / b;
        default:
            throw std::runtime_error("Unsupported op between float and int!");
    }
}

float op_int_float(OpType op, int a, float b) {
    switch (op) {
        case OpType::ADD:
            return a + b;
        case OpType::SUB:
            return a - b;
        case OpType::MUL:
            return a * b;
        case OpType::DIV:
            return a / b;
        default:
            throw std::runtime_error("Unsupported op between int and float!");
    }
}

Object* binop(OpType op, Object* left, Object* right) {
    IntegerObject* right_int = dynamic_cast<IntegerObject*>(right);
    IntegerObject* left_int = dynamic_cast<IntegerObject*>(left);
    if (right_int != nullptr && left_int != nullptr) {
        return new IntegerObject(op_int_int(op, left_int->value, right_int->value));
    }
    FloatObject* right_float = dynamic_cast<FloatObject*>(right);
    FloatObject* left_float = dynamic_cast<FloatObject*>(left);
    if (right_float != nullptr && left_float != nullptr) {
        return new FloatObject(op_float_float(op, left_float->value, right_float->value));
    }
    if (left_float != nullptr && right_int != nullptr) {
        return new FloatObject(op_float_int(op, left_float->value, right_int->value));
    }
    if (left_int != nullptr && right_float != nullptr) {
        return new FloatObject(op_int_float(op, left_int->value, right_float->value));
    }

    StringObject* right_str = dynamic_cast<StringObject*>(right);
    StringObject* left_str = dynamic_cast<StringObject*>(left);
    if (right_str != nullptr && left_str != nullptr) {
        return new StringObject(op_str_str(op, left_str->str, right_str->str));
    }
    ListObject* right_list = dynamic_cast<ListObject*>(right);
    ListObject* left_list = dynamic_cast<ListObject*>(left);
    ListObject* result;
    if (right_list != nullptr && left_list != nullptr) {
        if (op == OpType::ADD) {
            std::vector<Object*> elements;
            elements.insert(elements.end(), left_list->list.begin(), left_list->list.end());
            elements.insert(elements.end(), right_list->list.begin(), right_list->list.end());
            return new ListObject(elements);
        }
    } else {
        NoneObject* right_none = dynamic_cast<NoneObject*>(right);
        if (right_none != nullptr) {
            NoneObject* left_none = dynamic_cast<NoneObject*>(left);

        } else {
            throw std::runtime_error("Try to do a binop with two non-Integers or non-Strings");
        }
    }
}

void CodeRunner::visit(BinopInst& inst) {
//    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    Object* right = this->stack.pop();
    Object* left = this->stack.pop();
    Object* result = binop(inst.op, left, right);
    ObjectStore::register_object(result);
    this->stack.push(result);
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
//    Object* value = this->env->get(inst.identifier);
    Object* value = this->env->get_with_location(inst.identifier, inst.location);
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
    StringObject* str = dynamic_cast<StringObject*>(obj);

    if (list == nullptr && str == nullptr) {
        throw std::runtime_error("Subscript of not a List or String");
    }

    IntegerObject* index = this->stack.pop_integer();
    if (list != nullptr) {
        this->stack.push(list->list[index->value]);
    } else if (str != nullptr) {
        if (index->value > str->str.size()) {
            throw std::runtime_error("String index out of bounds!");
        }
        char c = str->str[index->value];
        StringObject* result = new StringObject(std::string(&c));
        ObjectStore::register_object(result);
        this->stack.push(result);
    }
    this->inst_ptr++;
}

void CodeRunner::visit(PushFunctionInst& inst) {
}

void CodeRunner::visit(PushIntegerInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    IntegerObject* obj = new IntegerObject(inst.num);
    ObjectStore::register_object(obj);
    this->stack.push(obj);
    this->inst_ptr++;
}

void CodeRunner::visit(PushStringInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    StringObject* obj = new StringObject(inst.str);
    ObjectStore::register_object(obj);
    this->stack.push(obj);
    this->inst_ptr++;
}

void CodeRunner::visit(ReturnInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    this->inst_ptr = this->code.size() + 1;
}

void CodeRunner::visit(SetInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    Object* value = this->stack.pop();
//    this->env->set(inst.identifier, value);
    this->env->set_with_location(inst.identifier, value, inst.location);
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
    ObjectStore::register_object(obj);
    this->stack.push(obj);
    this->inst_ptr++;
}

void CodeRunner::visit(MakeListInst& inst) {
    //std::cerr << "Run [" << inst.to_string() << "]" << std::endl;

    std::vector<Object*> list(inst.length, nullptr);
    for (int i = inst.length - 1; i >= 0; i--) {
        list[i] = this->stack.pop();
    }
    Object* obj = new ListObject(list);
    ObjectStore::register_object(obj);
    this->stack.push(obj);
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
    Object* obj = new BooleanObject(inst.boolean);
    ObjectStore::register_object(obj);
    this->stack.push(obj);
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
//            VectorOfStrings names;
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
    const std::vector<Object*>& root = this->env->get_directly_reachable();
    ObjectStore::gc(root);
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
    Object* obj = new NoneObject();
    if (tos->equal(obj)) {
        this->stack.pop();
        this->inst_ptr += inst.offset;
    } else {
        this->inst_ptr++;
    }
    delete obj;
}

void CodeRunner::visit(PushNone& inst) {
    Object* obj = new NoneObject();
    ObjectStore::register_object(obj);
    this->stack.push(obj);
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
    TupleObject* obj = new TupleObject(tuple);
    ObjectStore::register_object(obj);
    this->stack.push(obj);
    this->inst_ptr++;
}

void CodeRunner::visit(PushFloatInst& inst) {
    FloatObject* obj = new FloatObject(inst.value);
    ObjectStore::register_object(obj);
    this->stack.push(obj);
    this->inst_ptr++;
}
