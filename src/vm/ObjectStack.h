//
// Created by chris on 21/7/20.
//

#ifndef OBJECTSTACK_H
#define OBJECTSTACK_H

#include <vector>
#include <stack>
#include <stdexcept>
#include "Object.h"
#include "IntegerObject.h"
#include "StringObject.h"
#include "ListObject.h"
#include "TupleObject.h"

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

    TupleObject* pop_tuple();

    StringObject* pop_string();
    ListObject* pop_list();

    Object* pop();

    bool empty();
};


#endif //OBJECTSTACK_H
