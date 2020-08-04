//
// Created by chris on 3/8/20.
//
#include "CodeObject.h"
#include "Object.h"

CodeObject::CodeObject(CodeBuiltin* builtin) {
    this->type = CodeType::BUILTIN;
    this->builtin = builtin;
}

bool CodeObject::equal(const Object* other) const {
    const CodeObject* other_ptr = dynamic_cast<const CodeObject*>(other);
    return true;
}

