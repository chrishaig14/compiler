//
// Created by chris on 17/12/20.
//

#ifndef UNTITLED1_GC_H
#define UNTITLED1_GC_H

#include <iostream>
#include "xobject.h"

class Scope {
public:
    Scope(Scope* parent) {
        this->parent = parent;
    }

    std::unordered_map<std::string, XObject*> vars;
    // std::vector<XObject*> reachable;

    void declare(const std::string& n, XObject* v) {
        this->vars[n] = v;
    }

    void set(const std::string& n, XObject* v) {
        if (this->vars.count(n) == 0) {
            this->parent->set(n, v);
        } else {
            this->vars[n] = v;
        }
    }

    void get_reachable() {
        // this->reachable.resize(this->vars.size(), nullptr);
        // int c = 0;
        for (auto v: this->vars) {
            XObject* obj = v.second;
            obj->set_reachable();
            // this->reachable[c] = obj;
            // c++;
            // if (obj->is_reachable()) {
            //     already in reachable list, don't add again
            // } else {
            //     obj->set_reachable();
            //     this->reachable[c] = obj;
            //     c++;
            // }
        }
        if (this->parent != nullptr) {
            this->parent->get_reachable();
            // this->reachable.insert(this->reachable.end(), parent_reachable.begin(), parent_reachable.end());
        }
        // return this->reachable;
        // int c = 0;
        // std::vector<XObject*> extra(this->vars.size(), nullptr);
        // for (auto v: this->vars) {
        //     XObject* obj = v.second;
        //     if (obj->is_reachable()) {
        //         // already in reachable list, don't add again
        //     } else {
        //         obj->set_reachable();
        //         extra[c] = obj;
        //         c++;
        //     }
        // }
        // this->reachable = std::vector<XObject*>(extra.begin(), extra.begin() + c);
        // if (this->parent != nullptr) {
        //     const std::vector<XObject*>& reachable_parent = this->parent->get_reachable();
        //     std::vector<XObject*> extra_p(this->vars.size(), nullptr);
        //     int k = 0;
        //     for (auto obj: reachable_parent) {
        //         if (obj->is_reachable()) {
        //             // already in reachable list, don't add again
        //         } else {
        //             obj->set_reachable();
        //             extra_p[k] = obj;
        //             k++;
        //         }
        //     }
        //     this->reachable.insert(this->reachable.end(), extra_p.begin(), extra_p.begin() + k);
        // }

        // return this->
        //         reachable;
    }

    Scope* parent;
// std::vector<XObject*> objects;
};

class GC {
    static std::vector<XObject*> all_objects;
    static Scope* current_scope;
    static Scope* current_function_scope;
    static std::vector<Scope*> function_scopes;
public:
    static void enter_scope();

    static void enter_function();

    static XObject* function_return(XObject* f);

    static void declare(const std::string& n, XObject* obj);

    static void set(const std::string& n, XObject* obj);

    static void leave_scope();


    static XObject* register_object(XObject* u);

    static void mark(const std::vector<XObject*>& root);

    static void sweep();

};


// std::vector<XObject*>* GC::survivors = new std::vector<XObject*>();

#endif //UNTITLED1_GC_H
