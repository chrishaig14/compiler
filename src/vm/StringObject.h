//
// Created by chris on 3/8/20.
//

#ifndef UNTITLED1_STRINGOBJECT_H
#define UNTITLED1_STRINGOBJECT_H

#include <string>
#include "Object.h"

class StringObject : public Object {
public:
    StringObject(std::string str) {
        this->str = str;
    }

    StringObject();

    bool equal(const Object* other) const override {
        const StringObject* other_ptr = dynamic_cast<const StringObject*>(other);
        return this->str == other_ptr->str;
    }

    std::string str;
};

#endif //UNTITLED1_STRINGOBJECT_H
