//
// Created by chris on 17/9/20.
//

#ifndef UNTITLED1_BUILTINLISTINTEGERTOSTRING_H
#define UNTITLED1_BUILTINLISTINTEGERTOSTRING_H


#include "Object.h"
#include "ObjectStack.h"
#include "IntegerObject.h"
#include "StringObject.h"

class BuiltinListIntegerToString : public CodeBuiltin {
public:
    BuiltinListIntegerToString();

    void run(ObjectStack& stack) override;

};


#endif //UNTITLED1_BUILTINLISTINTEGERTOSTRING_H
