//
// Created by chris on 3/8/20.
//

#ifndef CODEOBJECT_H
#define CODEOBJECT_H

#include "Object.h"
#include "CodeUser.h"

class CodeObject : public Object {
public:
    CodeType type;
    union {
        CodeUser* user;
        CodeBuiltin* builtin;
    } stuff;

    explicit CodeObject(CodeBuiltin builtin);

    explicit CodeObject(CodeUser* user_code);

    bool equal(const Object* other) const override;
};


#endif //CODEOBJECT_H
