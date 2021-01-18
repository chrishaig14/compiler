//
// Created by chris on 5/8/20.
//

#ifndef USEROBJECT_H
#define USEROBJECT_H


#include <string>
#include "Object.h"

class UserObject : public Object {
public:
    bool equal(const Object* other) const override;

    std::unordered_map<std::string, Object*> fields;

    UserObject(std::string type, VectorOfStrings fields);

    std::string type;
};


#endif //USEROBJECT_H
