//
// Created by chris on 21/7/20.
//

#include <iostream>
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
    if (this->_top == nullptr) {
        throw std::runtime_error("Popping from empty stack!");
    }
    Object* return_value = this->_top->value;
    this->_top = this->_top->previous;
    return return_value;
}

bool ObjectStack::empty() {
    return this->_top == nullptr;
}

IntegerObject* ObjectStack::pop_integer() {
    auto ptr = dynamic_cast<IntegerObject*>(this->pop());
    if (ptr == nullptr) {
        throw std::runtime_error("Trying to pop an integer, but it's not!");
    }
    return ptr;
}

StringObject* ObjectStack::pop_string() {
    auto ptr = dynamic_cast<StringObject*>(this->pop());
    if (ptr == nullptr) {
        throw std::runtime_error("Trying to pop a string, but it's not!");
    }
    return ptr;
}

ListObject* ObjectStack::pop_list() {
    auto ptr = dynamic_cast<ListObject*>(this->pop());
    if (ptr == nullptr) {
        throw std::runtime_error("Trying to pop a list, but it's not!");
    }
    return ptr;
}

TupleObject* ObjectStack::pop_tuple() {
    auto ptr = dynamic_cast<TupleObject*>(this->pop());
    if (ptr == nullptr) {
        throw std::runtime_error("Trying to pop a tuple, but it's not!");
    }
    return ptr;
}

ObjectStackElement::ObjectStackElement(Object* value, ObjectStackElement* previous) {
    this->value = value;
    this->previous = previous;
}
