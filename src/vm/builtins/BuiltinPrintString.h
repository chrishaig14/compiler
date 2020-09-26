//
// Created by chris on 4/9/20.
//

#ifndef UNTITLED1_BUILTINPRINTSTRING_H
#define UNTITLED1_BUILTINPRINTSTRING_H


#include "../Object.h"

class BuiltinPrintString : public CodeBuiltin {
public:
    void run(ObjectStack& stack) override;

    BuiltinPrintString() {
        this->ftype = new FunctionTypeNode({T_STRING}, T_INT);
    }

};


#endif //UNTITLED1_BUILTINPRINTSTRING_H
