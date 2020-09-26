//
// Created by chris on 24/9/20.
//

#include "BuiltinListLength.h"

#include "../ListObject.h"

void BuiltinListLength::run(ObjectStack& stack) {
    ListObject* list = dynamic_cast<ListObject*>(stack.pop());
    stack.push(new IntegerObject(list->list.size()));
}

BuiltinListLength::BuiltinListLength() {
    this->ftype = new FunctionTypeNode({T_LIST(TYPE("a", {}))}, T_INT);
}
