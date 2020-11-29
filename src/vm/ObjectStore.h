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

    static void gc(std::unordered_set<Object*>& root) {
        for (auto o: root) {
            o->visited = true;
        }
//    std::cout << "CALLING GARBAGE COLLECTION with root: " << std::endl;
//    for (auto p: root) {
//        std::cout << p->to_string() << " POINTER: " << p << std::endl;
//    }
//        std::unordered_set<Object*> visited;
//        ObjectStore::mark(root, visited);
        ObjectStore::mark(root);
        std::unordered_set<Object*> reachable;
        for (auto o: all_objects) {
            if (o->visited) {
                reachable.insert(o);
            } else {
                delete o;
            }
        }
//        for (auto e: visited) {
//            unreachable.erase(e);
//        }
//    std::set_difference(
//            all_objects.begin(),
//            all_objects.end(),
//            visited.begin(),
//            visited.end(),
//            std::inserter(unreachable, unreachable.end()));
//        for (auto o: unreachable) {
//            delete o;
//        }
//        all_objects = visited;

        all_objects = reachable;
        for (auto o: all_objects) {
            o->visited = false;
        }
    }

    static void mark(std::unordered_set<Object*>& root) {

//    static void mark(std::unordered_set<Object*>& root, std::unordered_set<Object*>& visited) {
        std::unordered_set<Object*> next_root;
        bool any = false;
        for (auto obj: root) {
//            visited.insert(obj);
            obj->visited = true;
            UserObject* user_obj = dynamic_cast<UserObject*>(obj);
            ListObject* list_obj = dynamic_cast<ListObject*>(obj);
            if (user_obj != nullptr) {
                for (auto m: user_obj->fields) {
                    if (!m.second->visited) {
                        next_root.insert(m.second);
                        any = true;
                    }
//                    if (visited.find(m.second) == visited.end()) {
//                        next_root.insert(m.second);
//                    }
                }
            } else if (list_obj != nullptr) {
                for (auto e: list_obj->list) {
                    if (!e->visited) {
                        next_root.insert(e);
                        any = true;
                    }
//                    if (visited.find(e) == visited.end()) {
//                        next_root.insert(e);
//                    }
                }
            } else {

            }
        }
        if (any) {
//            ObjectStore::mark(next_root, visited);
            ObjectStore::mark(next_root);
        }
    }

};


#endif //UNTITLED1_OBJECTSTORE_H
