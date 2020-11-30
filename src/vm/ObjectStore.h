//
// Created by chris on 29/11/20.
//

#ifndef UNTITLED1_OBJECTSTORE_H
#define UNTITLED1_OBJECTSTORE_H


#include <unordered_set>
#include "Object.h"
#include "UserObject.h"
#include "ListObject.h"



class ObjectStore {
private:
    static std::unordered_set<Object*> all_objects;
public:
    static void register_object(Object* obj) {
        ObjectStore::all_objects.insert(obj);
    }

    static void gc(const std::vector<Object*>& root) {
        for (auto o: root) {
//            o->visited = true;
//            std::cout << "flags: " << (int)o->flags << std::endl;
            SET_FLAG(o->flags, VISITED);
//            o->flags |= VISITED;
//            std::cout << "VISITED: " << (int)VISITED << std::endl;
//            std::cout << "flags: " << (int)o->flags << std::endl;
//            assert(FLAG_IS_SET(o->flags, VISITED));
        }
        ObjectStore::mark(root);
        std::unordered_set<Object*> reachable;
        for (auto o: all_objects) {
//            if (o->visited) {
            if (FLAG_IS_SET(o->flags, VISITED)) {
                reachable.insert(o);
            } else {
                delete o;
            }
        }
        all_objects = reachable;
        for (auto o: all_objects) {
//            o->visited = false;
            UNSET_FLAG(o->flags, VISITED | INSERTED);
        }
    }

    static void mark(const std::vector<Object*>& root) {
        std::vector<Object*> next_root;
        bool any = false;
        for (auto obj: root) {
//            obj->visited = true;
            SET_FLAG(obj->flags, VISITED);
//            ListObject* list_obj = dynamic_cast<ListObject*>(obj);
//            UserObject* user_obj = dynamic_cast<UserObject*>(obj);
//            if (user_obj != nullptr) {
            if (FLAG_IS_SET(obj->flags, IS_USER)) {
                UserObject* user_obj = dynamic_cast<UserObject*>(obj);
                for (auto m: user_obj->fields) {
//                    if (!m.second->visited) {
                    if (!FLAG_IS_SET(m.second->flags, VISITED) && !FLAG_IS_SET(m.second->flags, INSERTED)) {
                        next_root.push_back(m.second);
                        any = true;
                    }
                }
//            } else if (list_obj != nullptr) {
            } else if (FLAG_IS_SET(obj->flags, IS_LIST)) {
                ListObject* list_obj = dynamic_cast<ListObject*>(obj);
                for (auto e: list_obj->list) {
//                    if (!e->visited) {
                    if (!FLAG_IS_SET(e->flags, VISITED) && !FLAG_IS_SET(e->flags, INSERTED)) {
                        next_root.push_back(e);
                        any = true;
                    }
                }
            } else {

            }
        }
        if (any) {
            ObjectStore::mark(next_root);
        }
    }

};


#endif //UNTITLED1_OBJECTSTORE_H
