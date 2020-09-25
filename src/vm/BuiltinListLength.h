//
// Created by chris on 24/9/20.
//

#ifndef UNTITLED1_BUILTINLISTLENGTH_H
#define UNTITLED1_BUILTINLISTLENGTH_H


#include "Object.h"
#include "ObjectStack.h"
#include "IntegerObject.h"
#include "StringObject.h"


class BuiltinListLength : public CodeBuiltin {
public:
    BuiltinListLength();

    void run(ObjectStack& stack) override;

};


#endif //UNTITLED1_BUILTINLISTLENGTH_H
