//
// Created by chris on 20/7/20.
//

#include "Value.h"
#include "FirstPass.h"

Value::Value(Integer integer) {
    this->integer = integer;
    this->type = ValueType::INTEGER;
}

bool equal(Value* a, Value* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    if (a->type != b->type) return false;
    switch (a->type) {
        case ValueType::INTEGER:
            return a->integer.value == b->integer.value;
    }
    return false;
}

Integer::Integer() {}

Integer::Integer(int value) {
    this->value = value;
}
