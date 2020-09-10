//
// Created by chris on 3/8/20.
//

#ifndef UNTITLED1_CODEOBJECT_H
#define UNTITLED1_CODEOBJECT_H

#include "Object.h"
#include "CodeUser.h"

class CodeObject : public Object {
public:
    CodeType type;
    union {
        CodeUser* user;
        CodeBuiltin* builtin;
    };

    CodeObject(CodeBuiltin* builtin);

    CodeObject(CodeUser* user_code);

    bool equal(const Object* other) const override;
};


#endif //UNTITLED1_CODEOBJECT_H
