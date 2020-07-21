//
// Created by chris on 21/7/20.
//

#include "ValueStack.h"

ValueStack::ValueStack() {
    this->_top = nullptr;
}

Value* ValueStack::top() {
    return this->_top->value;
}

void ValueStack::push(Value* value) {
    ValueStackElement* new_top = new ValueStackElement(value, this->_top);
    this->_top = new_top;
}

Value* ValueStack::pop() {
    if (this->_top == nullptr) throw std::runtime_error("Popping from empty stack!");
    Value* return_value = this->_top->value;
    this->_top = this->_top->previous;
    return return_value;
}

bool ValueStack::empty() {
    return this->_top == nullptr;
}

ValueStackElement::ValueStackElement(Value* value, ValueStackElement* previous) {
    this->value = value;
    this->previous = previous;
}
