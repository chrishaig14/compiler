//
// Created by chris on 3/8/20.
//

#ifndef UNTITLED1_BUILTINSUM_H
#define UNTITLED1_BUILTINSUM_H

#include "../ObjectStack.h"
#include "../IntegerObject.h"

class BuiltinSum : public CodeBuiltin {
public:
    void run(ObjectStack& stack);
};

void BuiltinSum::run(ObjectStack& stack) {
    Object* value_a = stack.pop();
    Object* value_b = stack.pop();
    stack.push(value_a->sum(value_b));
}

#endif //UNTITLED1_BUILTINSUM_H
