//
// Created by chris on 5/8/20.
//

#ifndef UNTITLED1_USEROBJECT_H
#define UNTITLED1_USEROBJECT_H


#include <string>
#include "Object.h"

class UserObject : public Object {
public:
    bool equal(const Object* other) const override;

    std::map<std::string, Object*> fields;

    UserObject(std::string type, std::vector<std::string> fields) {
        this->type = type;
        for (auto f: fields) {
            this->fields[f] = nullptr;
        }
    }

    std::string type;
};


#endif //UNTITLED1_USEROBJECT_H
