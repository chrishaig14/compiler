//
// Created by chris on 21/7/20.
//

#ifndef UNTITLED1_VALUESTACK_H
#define UNTITLED1_VALUESTACK_H

#include <vector>
#include <stack>
#include <stdexcept>
#include "Inst.h"
#include "Value.h"

class ValueStackElement {
public:
    Value* value;
    ValueStackElement* previous;

    ValueStackElement(Value* value, ValueStackElement* previous);

};

class ValueStack {
    ValueStackElement* _top;

public:
    ValueStack();

    Value* top();

    void push(Value* value);

    Value* pop();

    bool empty();
};


#endif //UNTITLED1_VALUESTACK_H
