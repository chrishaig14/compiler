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

    std::map<std::string, Object*> fields;

    UserObject(std::string type, std::vector<std::string> fields);

    std::string type;
};


#endif //USEROBJECT_H
