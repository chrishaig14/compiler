//
// Created by chris on 17/9/20.
//

#include "BuiltinListIntegerToString.h"
#include "../ListObject.h"

void BuiltinListIntegerToString::run(ObjectStack& stack) {
    ListObject* list = dynamic_cast<ListObject*>(stack.pop());
    std::string s = "[";
    for (int i = 0; i < list->list.size(); i++) {
        IntegerObject* el = dynamic_cast<IntegerObject*>(list->list[i]);
        s += std::to_string(el->value) + ", ";
    }
    s = s.substr(0, s.size() - 2);
    s += "]";
    stack.push(new StringObject(s));
}

BuiltinListIntegerToString::BuiltinListIntegerToString() {
    this->ftype = new FunctionTypeNode({T_LIST(T_INT)}, T_STRING);
}
