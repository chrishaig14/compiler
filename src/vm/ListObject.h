//
// Created by chris on 5/8/20.
//

#ifndef UNTITLED1_LISTOBJECT_H
#define UNTITLED1_LISTOBJECT_H


#include "Object.h"

class ListObject : public Object {
public:
    bool equal(const Object* other) const override;

    ListObject(std::vector<Object*> list) { this->list = list; }

    std::vector<Object*> list;
};


#endif //UNTITLED1_LISTOBJECT_H
