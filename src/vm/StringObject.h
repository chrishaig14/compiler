//
// Created by chris on 3/8/20.
//

#ifndef STRINGOBJECT_H
#define STRINGOBJECT_H

#include <string>
#include "Object.h"

class StringObject : public Object {
public:
    StringObject(std::string str);

    StringObject();

    bool equal(const Object* other) const override;

    std::string str;
};

#endif //STRINGOBJECT_H
