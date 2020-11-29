//
// Created by chris on 29/11/20.
//

#ifndef UNTITLED1_OBJECTSTORE_H
#define UNTITLED1_OBJECTSTORE_H


#include <unordered_set>
#include "Object.h"

class ObjectStore {
private:
    static std::unordered_set<Object*> all_objects;
public:
    static void register_object(Object* obj) {
        ObjectStore::all_objects.insert(obj);
    }
};


#endif //UNTITLED1_OBJECTSTORE_H
