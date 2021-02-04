//
// Created by chris on 23/11/20.
//

#ifndef UNTITLED1_TUPLEOBJECT_H
#define UNTITLED1_TUPLEOBJECT_H


#include <vector>
#include "Object.h"

class TupleObject : public Object {
public:

    std::vector<Object*> values;

    TupleObject(std::vector<Object*> values);

    bool equal(const Object* other) const override;
};


#endif //UNTITLED1_TUPLEOBJECT_H
