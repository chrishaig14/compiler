//
// Created by chris on 4/9/20.
//

#include <iostream>
#include "BuiltinPrintString.h"
#include "ObjectStack.h"
#include "StringObject.h"

void BuiltinPrintString::run(ObjectStack& stack) {
    Object* value = stack.pop();
    StringObject* integer = dynamic_cast<StringObject*>(value);
    if (integer == nullptr) {
        throw std::runtime_error("trying to print a non integer!");
    }
    std::cout <<  "printing " << integer->str << std::endl;
}
