//
// Created by chris on 21/7/20.
//

#include "ObjectStack.h"

ObjectStack::ObjectStack() {
    this->_top = nullptr;
}

Object* ObjectStack::top() {
    return this->_top->value;
}

void ObjectStack::push(Object* value) {
    ObjectStackElement* new_top = new ObjectStackElement(value, this->_top);
    this->_top = new_top;
}

Object* ObjectStack::pop() {
    if (this->_top == nullptr) throw std::runtime_error("Popping from empty stack!");
    Object* return_value = this->_top->value;
    this->_top = this->_top->previous;
    return return_value;
}

bool ObjectStack::empty() {
    return this->_top == nullptr;
}

ObjectStackElement::ObjectStackElement(Object* value, ObjectStackElement* previous) {
    this->value = value;
    this->previous = previous;
}
