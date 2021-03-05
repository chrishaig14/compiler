//
// Created by chris on 17/12/20.
//

#ifndef UNTITLED1_GC_H
#define UNTITLED1_GC_H

#include <iostream>
#include "xobject.h"

class Scope {
public:
    bool is_function;
    int temp;

    Scope(Scope* parent) {
        is_function = false;
        this->temp = 0;
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

class Frame {
    std::string function_name;

public:
    Scope* scope;

    Frame(const std::string& name) {
        this->function_name = name;
        this->scope = nullptr;
        this->num_args = 0;
    }

    ~Frame() {
        Scope* cur_scope = this->scope;
        while (cur_scope != nullptr) {
            Scope* old_scope = cur_scope;
            cur_scope = cur_scope->parent;
            delete old_scope;
        }
    }

    int num_args;
};


class GC {
    static std::vector<XObject*> all_objects;
    static Scope* current_scope;
    static Frame* frame;
    static std::vector<Frame*> frames;
public:
    static void enter_local_scope();

    static void enter_function(std::string function_name);

    static TaggedObject* function_return(TaggedObject* f);

    static void declare(const std::string& n, TaggedObject* obj);

    static void set(const std::string& n, TaggedObject* obj);

    static void leave_local_scope();


    static TaggedObject* register_object(TaggedObject* u);

    static void mark(const std::vector<XObject*>& root);

    static void sweep();

    static void leave_scope(Scope* parent);

    static TaggedObject* temp(TaggedObject* obj);
};


// std::vector<XObject*>* GC::survivors = new std::vector<XObject*>();

#endif //UNTITLED1_GC_H
