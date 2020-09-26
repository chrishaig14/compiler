//
// Created by chris on 21/7/20.
//

#ifndef UNTITLED1_OBJECTSTACK_H
#define UNTITLED1_OBJECTSTACK_H

#include <vector>
#include <stack>
#include <stdexcept>
#include "Object.h"
#include "IntegerObject.h"
#include "StringObject.h"
#include "ListObject.h"

class ObjectStackElement {
public:
    Object* value;
    ObjectStackElement* previous;

    ObjectStackElement(Object* value, ObjectStackElement* previous);

};

class ObjectStack {
    ObjectStackElement* _top;

public:
    ObjectStack();

    Object* top();

    void push(Object* value);

    IntegerObject* pop_integer();
    StringObject* pop_string();
    ListObject* pop_list();

    Object* pop();

    bool empty();
};


#endif //UNTITLED1_OBJECTSTACK_H
