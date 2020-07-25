//
// Created by chris on 20/7/20.
//

#include "Value.h"
#include "FirstPass.h"

Value::Value(Integer* integer) {
    this->integer = integer;
    this->type = ValueType::INTEGER;
}

Value::Value(Code* code) {
    this->code = code;
    this->type = ValueType::CODE;
}

bool equal(Value* a, Value* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    if (a->type != b->type) return false;
    switch (a->type) {
        case ValueType::INTEGER:
            return a->integer->value == b->integer->value;
        case ValueType::CODE:
            throw std::runtime_error("Error in equals: cannot compare two code objects!");
            break;
    }
    return false;
}

Integer::Integer() {}

Integer::Integer(int value) {
    this->value = value;
}

void BuiltinSum::run(ValueStack* stack) {
    Value* value_a = stack->pop();
    Value* value_b = stack->pop();
    if (value_a->type == ValueType::INTEGER and value_b->type == ValueType::INTEGER) {
        stack->push(new Value(new Integer(value_a->integer->value + value_b->integer->value)));
    }
}

size_t CodeUser::size() {
    return this->code.size();
}

Inst* CodeUser::get(int i) {
    return this->code[i];
}

CodeUser::CodeUser(std::vector<Inst*> code) {
    this->code = code;
}

Code::Code(CodeBuiltin* builtin) {
    this->type = CodeType::BUILTIN;
    this->builtin = builtin;
}
