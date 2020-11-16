//
// Created by chris on 3/8/20.
//
#include "CodeObject.h"
#include "Object.h"

CodeObject::CodeObject(CodeBuiltin builtin) {
    this->type = CodeType::BUILTIN;
    this->stuff.builtin = new CodeBuiltin(builtin);
}

bool CodeObject::equal(const Object* other) const {
    const CodeObject* other_ptr = dynamic_cast<const CodeObject*>(other);
    return true;
}

CodeObject::CodeObject(CodeUser* user_code) {
    this->type = CodeType::USER;
    this->stuff.user = user_code;
}

