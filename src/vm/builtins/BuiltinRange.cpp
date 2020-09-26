//
// Created by chris on 25/9/20.
//
#include <iostream>
#include "BuiltinRange.h"
#include "../ObjectStack.h"
#include "../StringObject.h"

void BuiltinRange::run(ObjectStack& stack) {
    IntegerObject* end = stack.pop_integer();
    IntegerObject* step = stack.pop_integer();
    IntegerObject* start = stack.pop_integer();
    std::vector<Object*> result;
    for (int i = start->value; i < end->value; i += step->value) {
        result.push_back(new IntegerObject(i));
    }
    stack.push(new ListObject(result));
}

BuiltinRange::BuiltinRange() {
    this->ftype = new FunctionTypeNode({T_INT, T_INT, T_INT}, T_LIST(T_INT));
}
