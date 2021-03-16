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

    void declare(const std::string& n, XObject* v) {
        this->vars[n] = v;
    }

    TaggedObject* set(TaggedObject* old_value_t, TaggedObject* new_value_t) {
        if (has_tag(old_value_t, OBJECT_TAG)) {
            XObject* old_value = UNTAG(old_value_t);
            old_value->count--;
            if (old_value->count == 0) {
                delete old_value;
            }
            XObject* new_value = UNTAG(new_value_t);
            new_value->count++;
        }
        return new_value_t;
    }

    Scope* parent;
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

    static TaggedObject* declare(TaggedObject* obj);

    static void set(const std::string& n, TaggedObject* obj);

    static void leave_local_scope();


    static TaggedObject* temp(TaggedObject* obj);
    static TaggedObject* inc(TaggedObject* pObject);
    static TaggedObject* assign(TaggedObject* old_value_t, TaggedObject* new_value_t);
    static void out_of_scope(TaggedObject* old_value_t);
};


// std::vector<XObject*>* GC::survivors = new std::vector<XObject*>();

#endif //UNTITLED1_GC_H
