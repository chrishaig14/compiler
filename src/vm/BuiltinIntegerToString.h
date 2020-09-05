//
// Created by chris on 3/9/20.
//

#ifndef UNTITLED1_BUILTININTEGERTOSTRING_H
#define UNTITLED1_BUILTININTEGERTOSTRING_H


#include "Object.h"

class BuiltinIntegerToString : public CodeBuiltin {
public:
    BuiltinIntegerToString();

    void run(ObjectStack& stack) override;

};


#endif //UNTITLED1_BUILTININTEGERTOSTRING_H
