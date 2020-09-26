//
// Created by chris on 25/9/20.
//

#ifndef UNTITLED1_BUILTINRANGE_H
#define UNTITLED1_BUILTINRANGE_H


#include "../ObjectStack.h"

class BuiltinRange : public CodeBuiltin {
public:
    void run(ObjectStack& stack) override;

    BuiltinRange();

};

#endif //UNTITLED1_BUILTINRANGE_H
