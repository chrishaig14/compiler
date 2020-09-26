//
// Created by chris on 3/9/20.
//

#include <iostream>
#include "BuiltinIntegerToString.h"
#include "../ObjectStack.h"
#include "../IntegerObject.h"
#include "../StringObject.h"

void BuiltinIntegerToString::run(ObjectStack& stack) {
    Object* value = stack.pop();
    IntegerObject* integer = dynamic_cast<IntegerObject*>(value);
    stack.push(new StringObject(std::to_string(integer->value)));
}

BuiltinIntegerToString::BuiltinIntegerToString() {
    this->ftype = new FunctionTypeNode({T_INT}, T_STRING);
}
