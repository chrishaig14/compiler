//
// Created by chris on 5/8/20.
//

#ifndef LISTOBJECT_H
#define LISTOBJECT_H


#include <vector>
#include "Object.h"

class ListObject : public Object {
public:
    bool equal(const Object* other) const override;

    ListObject(std::vector<Object*> list) { this->list = list; }

    std::vector<Object*> list;
};


#endif //LISTOBJECT_H
